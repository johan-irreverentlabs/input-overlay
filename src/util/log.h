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

#pragma once
#include <obs-module.h>

extern int recording_time_ms();

#define write_log(log_level, format, ...) blog(log_level, "[input-overlay] " format, ##__VA_ARGS__)
#define write_input_event(log_level, format, ...) blog(log_level, "[input-event] " format, ##__VA_ARGS__)
#define write_timed_input_event(format, ...) blog(log_level, "[input-event] " format, ##__VA_ARGS__)
#define bdebug(format, ...) write_log(LOG_DEBUG, format, ##__VA_ARGS__)
#define binfo(format, ...) write_log(LOG_INFO, format, ##__VA_ARGS__)
#define binput_event(format, ...) \
    do { \
        int recording_time = recording_time_ms(); \
        if (recording_time > 0) { \
            blog(LOG_INFO, "[input-event]{ time: %d, " format, recording_time, ##__VA_ARGS__); \
        } \
    } while (0)
#define bwarn(format, ...) write_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berr(format, ...) write_log(LOG_ERROR, format, ##__VA_ARGS__)
