/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "element_analog_stick.hpp"
#include "../../sources/input_source.hpp"
#include "SDL.h"
#include <keycodes.h>
#include "util/log.h"
extern Logger logger;

void element_analog_stick::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    m_side = static_cast<element_side>(obj[CFG_SIDE].toInt());
    m_radius = static_cast<int32_t>(obj[CFG_STICK_RADIUS].toInt());
    m_keycode = (m_side == element_side::LEFT) ? SDL_CONTROLLER_BUTTON_LEFTSTICK : SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
}

void element_analog_stick::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    auto pos = m_pos;
    gs_rect *temp = settings->is_pad_button_pressed(m_keycode) ? &m_pressed : &m_mapping;

    if (settings->gamepad || settings->remote_input_data) {
        if (m_side == element_side::LEFT) {
            pos.x += settings->pad_axis(SDL_CONTROLLER_AXIS_LEFTX) * m_radius;
            pos.y += settings->pad_axis(SDL_CONTROLLER_AXIS_LEFTY) * m_radius;
        } else {
            pos.x += settings->pad_axis(SDL_CONTROLLER_AXIS_RIGHTX) * m_radius;
            pos.y += settings->pad_axis(SDL_CONTROLLER_AXIS_RIGHTY) * m_radius;
        }
    }
    element_texture::draw(effect, image, temp, &pos);
}

void element_analog_stick::tick(float, sources::overlay_settings *settings)
{
    float fx = 0;
    float fy = 0;
    if (settings->gamepad || settings->remote_input_data) {
        if (m_side == element_side::LEFT) {
            fx = settings->pad_axis(SDL_CONTROLLER_AXIS_LEFTX);
            fy = settings->pad_axis(SDL_CONTROLLER_AXIS_LEFTY);
        } else {
            fx = settings->pad_axis(SDL_CONTROLLER_AXIS_RIGHTX);
            fy = settings->pad_axis(SDL_CONTROLLER_AXIS_RIGHTY);
        }
    }
    int x = 0;
    int y = 0;
    if (fx > 0.5) {
        x = 1;
    } else if (fx < -0.5) {
        x = -1;
    }

    if (fy > 0.5) {
        y = 1;
    } else if (fy < -0.5) {
        y = -1;
    }

    if (x != m_last_x) {
        if (m_side == element_side::LEFT) {
            logger.logMessage("left_stick_x", std::to_string(x));
        } else {
            logger.logMessage("right_stick_x", std::to_string(x));
        }
    }

    if (y != m_last_y) {
        if (m_side == element_side::LEFT) {
            logger.logMessage("left_stick_y", std::to_string(x));
        } else {
            logger.logMessage("right_stick_y", std::to_string(x));
        }

    }

    m_last_x = x;
    m_last_y = y;
}
