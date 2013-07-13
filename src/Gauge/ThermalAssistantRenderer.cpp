/*
  Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "ThermalAssistantRenderer.hpp"
#include "Util/Macros.hpp"
#include "Util/Clamp.hpp"
#include "NMEA/Attitude.hpp"
#include "NMEA/Derived.hpp"
#include "Screen/Canvas.hpp"
#include "Screen/Layout.hpp"
#include "Formatter/UserUnits.hpp"
#include "Language/Language.hpp"
#include "Look/ThermalAssistantLook.hpp"
#include "Interface.hpp"

#ifdef ENABLE_OPENGL
#include "Screen/OpenGL/Scope.hpp"
#endif

static constexpr fixed VISIBILITY_SCALE(5); // arbitrary multiplier for off-center value to make it better visible


/**
 * Returns the relative location of thermal center from polar lift distribution
 */
FixedPoint ThermalAssistantRenderer::LiftPoints::GetAverage() const
{
  FixedPoint avg = { fixed(0), fixed(0) };

  for (auto it = begin(), it_end = end(); it != it_end; ++it) {
    avg.x += fixed (it->x);
    avg.y += fixed (it->y);
  }

  avg.x /= size();
  avg.y /= size();

  return avg;
}

ThermalAssistantRenderer::ThermalAssistantRenderer(const ThermalAssistantLook &_look,
                                                   unsigned _padding, bool _small)
  :look(_look),
   padding(_padding),
   small(_small),
   direction(Angle::Zero()) {}

void
ThermalAssistantRenderer::Update(const AttitudeState &attitude,
                               const DerivedInfo &derived)
{
  direction = attitude.heading;
  circling = (CirclingInfo)derived;
  vario = (VarioInfo)derived;
}

fixed
ThermalAssistantRenderer::CalculateMaxLift() const
{
  return (*std::max_element(vario.lift_database.begin(),
                            vario.lift_database.end())).lift;
}

void
ThermalAssistantRenderer::CalculateLiftPoints(LiftPoints &lift_points,
                                            fixed max_lift) const
{
  Angle angle = -direction;
  constexpr Angle delta = Angle::FullCircle() / unsigned(std::tuple_size<LiftDatabase>());

  for (unsigned i = 0; i < lift_points.size(); i++, angle += delta) {
    auto sincos = angle.SinCos();
    auto scale = NormalizeLift(vario.lift_database[i].lift, max_lift) * fixed(radius);

    lift_points[i].x = (int)(sincos.second * scale);
    lift_points[i].y = (int)(sincos.first * scale);

    if (!circling.TurningLeft()) {
      lift_points[i].x *= -1;
      lift_points[i].y *= -1;
    }

    lift_points[i].x += mid.x;
    lift_points[i].y += mid.y;
  }
}

fixed
ThermalAssistantRenderer::NormalizeLift(fixed lift, fixed max_lift)
{
  lift = (lift + max_lift) / Double(max_lift);
  return Clamp(lift, fixed(0), fixed(1));
}

void
ThermalAssistantRenderer::PaintRadarPlane(Canvas &canvas) const
{
  canvas.Select(look.plane_pen);

  PixelScalar x = mid.x + (circling.TurningLeft() ? radius : -radius);

  canvas.DrawLine(x + Layout::FastScale(small ? 5 : 10),
              mid.y - Layout::FastScale(small ? 1 : 2),
              x - Layout::FastScale(small ? 5 : 10),
              mid.y - Layout::FastScale(small ? 1 : 2));
  canvas.DrawLine(x,
              mid.y - Layout::FastScale(small ? 3 : 6),
              x,
              mid.y + Layout::FastScale(small ? 3 : 6));
  canvas.DrawLine(x + Layout::FastScale(small ? 2 : 4),
              mid.y + Layout::FastScale(small ? 2 : 4),
              x - Layout::FastScale(small ? 2 : 4),
              mid.y + Layout::FastScale(small ? 2 : 4));
}

void
ThermalAssistantRenderer::PaintRadarBackground(Canvas &canvas, fixed max_lift, fixed max_lift_ceil) const
{
  canvas.SelectHollowBrush();

  canvas.Select(look.inner_circle_pen);
  canvas.DrawCircle(mid.x, mid.y, radius / 2);
  canvas.Select(look.outer_circle_pen);

  fixed mc = CommonInterface::GetComputerSettings().polar.glide_polar_task.GetMC();
  unsigned mc_radius = (radius / 2) + unsigned( mc / max_lift_ceil * fixed (radius / 2 ));

  if (circling.circling && mc_radius < radius)
    canvas.DrawCircle(mid.x, mid.y, mc_radius);

  if (small)
    return;

  canvas.SetTextColor(COLOR_BLACK);
  canvas.Select(look.circle_label_font);
  canvas.SetBackgroundColor(look.background_color);
  canvas.SetBackgroundOpaque();

  TCHAR lift_string[10];

  // draw 0 line text
  FormatUserVerticalSpeed(fixed(0), lift_string, ARRAY_SIZE(lift_string));
  PixelSize s = canvas.CalcTextSize(lift_string);
  canvas.DrawText(mid.x - s.cx / 2,
                  mid.y + radius / 2 - s.cy * 0.75,
                  lift_string);

  if (circling.circling) {
    TCHAR lift_string2[20];

    //draw max lift value text
    FormatUserVerticalSpeed(max_lift, lift_string, ARRAY_SIZE(lift_string));
    _stprintf(lift_string2, _T("max %s"), lift_string);
    s = canvas.CalcTextSize(lift_string2);
    canvas.DrawText(mid.x - s.cx / 2,
                    mid.y + radius - s.cy * 0.75,
                    lift_string2);

    // draw MC line value text
     if (mc_radius < radius) {
      FormatUserVerticalSpeed(mc, lift_string, ARRAY_SIZE(lift_string));
      _stprintf(lift_string2, _T("MC %s"), lift_string);
      s = canvas.CalcTextSize(lift_string2);
      canvas.DrawText(mid.x - s.cx / 2,
                      mid.y - mc_radius - s.cy * 0.5,
                      lift_string2);
    }
  }
  canvas.SetBackgroundTransparent();
}

void
ThermalAssistantRenderer::PaintPoints(Canvas &canvas,
                                    const LiftPoints &lift_points) const
{
#ifdef ENABLE_OPENGL
  GLBlend blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#elif defined(USE_GDI)
  canvas.SetMixMask();
#endif /* GDI */

  canvas.Select(look.polygon_brush);
  canvas.Select(look.polygon_pen);
  canvas.DrawPolygon(lift_points.data(), lift_points.size());
}

void
ThermalAssistantRenderer::PaintAdvisor(Canvas &canvas,
                                     const LiftPoints &lift_points) const
{
  FixedPoint avg = lift_points.GetAverage( );
  // scale the advisor point to make it bigger for better visibility
  avg.x = ((avg.x - fixed(mid.x)) * VISIBILITY_SCALE) + fixed(mid.x);
  avg.y = ((avg.y - fixed(mid.y)) * VISIBILITY_SCALE) + fixed(mid.y);

  // draw advisor arm and point
  RasterPoint rp((int) avg.x, (int) avg.y);
  canvas.Select(look.advisor_pen);
  canvas.DrawLine(mid, rp);
  canvas.DrawCircle (rp.x, rp.y, small?4:8);
}

void
ThermalAssistantRenderer::PaintNotCircling(Canvas &canvas) const
{
  if (small)
    return;

  const TCHAR* str = _("Not Circling");
  canvas.Select(look.overlay_font);
  PixelSize ts = canvas.CalcTextSize(str);
  canvas.SetTextColor(look.text_color);
  canvas.DrawText(mid.x - (ts.cx / 2), mid.y - (radius / 2), str);
}

void
ThermalAssistantRenderer::UpdateLayout(const PixelRect &rc)
{
  radius = std::min(rc.right - rc.left, rc.bottom - rc.top) / 2 - padding;
  mid = rc.GetCenter();
}

void
ThermalAssistantRenderer::Paint(Canvas &canvas)
{
  fixed max_lift = CalculateMaxLift();
  fixed max_lift_ceil = ceil(std::max(fixed(1),max_lift));

  PaintRadarBackground(canvas, max_lift, max_lift_ceil);
  if (!circling.circling) {
    PaintNotCircling(canvas);
    return;
  }

  LiftPoints lift_points;
  CalculateLiftPoints(lift_points, max_lift_ceil);
  PaintPoints(canvas, lift_points);
  PaintAdvisor(canvas, lift_points);

  PaintRadarPlane(canvas);
}
