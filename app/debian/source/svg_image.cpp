/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
 *  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
 *
 *  This program is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,   but WITHOUT
 *  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
 *
 *  You should have received a copy of the  GNU General Public License along with
 *  this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ==============================================================================
 */

#include "svg_image.h"

const char* svg_default      = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <rect x=\"3\" y=\"3\" width=\"18\" height=\"18\" rx=\"2\" ry=\"2\"/><line x1=\"3\" y1=\"12\" x2=\"21\" y2=\"12\"/><line x1=\"12\" y1=\"3\" x2=\"12\" y2=\"21\"/></svg>";
const char* svg_generic      = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" ><circle cx=\"12\" cy=\"12\" r=\"10\"></circle><line x1=\"12\" y1=\"16\" x2=\"12\" y2=\"12\"></line><line x1=\"12\" y1=\"8\" x2=\"12.01\" y2=\"8\"></line></svg>";
const char* svg_percent      = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <rect x=\"3\" y=\"3\" width=\"18\" height=\"18\" rx=\"2\" ry=\"2\"/><line x1=\"3\" y1=\"12\" x2=\"21\" y2=\"12\"/><line x1=\"12\" y1=\"3\" x2=\"12\" y2=\"21\"/></svg>";
const char* svg_rel_humidity = "<svg viewBox=\"0 0 28 28\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" > <path d=\"M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z\"></path></svg>";
const char* svg_abs_humidity = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"1\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M5 12c-1.1 0-2-.9-2-2 0-1.1.9-2 2-2h.06a4 4 0 0 1 7.88 0H15c1.1 0 2 .9 2 2s-.9 2-2 2H5z\" /><ellipse cx=\"7\" cy=\"21\" rx=\"1.1\" ry=\"1.5\"/><circle cx=\"7\" cy=\"19\" r=\"0.3\"/><circle cx=\"7.2\" cy=\"16\" r=\"0.2\"/><ellipse cx=\"12\" cy=\"19\" rx=\"1.1\" ry=\"1.5\"/><circle cx=\"12\" cy=\"17\" r=\"0.3\"/><circle cx=\"12.2\" cy=\"14\" r=\"0.2\"/></svg>";
const char* svg_air_pressure = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"24\" height = \"24\" > ><line x1=\"0\" y1=\"12\" x2=\"24\" y2=\"12\" stroke=\"black\" stroke-width=\"1\"/><line x1=\"12\" y1=\"0\" x2=\"12\" y2=\"10\" stroke=\"black\" stroke-width=\"1\"/><polygon points=\"12,14 10,16 14,16\" fill=\"none\" stroke=\"black\" stroke-width=\"1\"/><line x1=\"12\" y1=\"24\" x2=\"12\" y2=\"14\" stroke=\"black\" stroke-width=\"1\"/><polygon points=\"12,10 10,8 14,8\" fill=\"none\" stroke=\"black\" stroke-width=\"1\"/></svg>";
const char* svg_temperature  = "<svg viewBox=\"0 0 28 28\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" > <path d=\"M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z\"></path></svg>";
const char* svg_dewpoint     = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"0.2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"24\" height = \"24\" ><path d=\"M1.5 1v22\" stroke=\"currentColor\" stroke-width=\"0.8\"/><path d=\"M22.5 1v22\" stroke=\"currentColor\" stroke-width=\"0.8\"/><g fill=\"currentColor\"><ellipse cx=\"7\" cy=\"12\" rx=\"1.2\" ry=\"1.8\"/><circle cx=\"7\" cy=\"9\" r=\"0.3\"/><circle cx=\"6.8\" cy=\"7\" r=\"0.2\"/><ellipse cx=\"12\" cy=\"19\" rx=\"1.1\" ry=\"1.7\"/><circle cx=\"12\" cy=\"15\" r=\"0.35\"/><circle cx=\"12\" cy=\"12\" r=\"0.25\"/><ellipse cx=\"17\" cy=\"10\" rx=\"1.1\" ry=\"1.8\"/><circle cx=\"17\" cy=\"8\" r=\"0.3\"/><circle cx=\"17.2\" cy=\"5\" r=\"0.2\"/><ellipse cx=\"10\" cy=\"4\" rx=\"0.8\" ry=\"1.3\"/><ellipse cx=\"17\" cy=\"17\" rx=\"0.7\" ry=\"1.3\"/></g></svg>";
const char* svg_wind         = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" width = \"20\" height = \"20\" > <path d=\"M9.59 4.59A2 2 0 1 1 11 8H2m10.59 11.41A2 2 0 1 0 14 16H2m15.73-8.27A2.5 2.5 0 1 1 19.5 12H2\"></path></svg>";
const char* svg_voltage      = "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"" SVG_STROKE_COLOR "\" stroke-width=\"2\"> <circle cx=\"12\" cy=\"12\" r=\"10\"/><path d=\"M12 8v8M8 12h8\"/></svg>\n";

const char* svg_search       = "<svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" stroke=\"none\" fill=\"" SVG_STROKE_COLOR "\"><path fill-rule=\"evenodd\" d=\"M16.3198574,14.9056439 L21.7071068,20.2928932 L20.2928932,21.7071068 L14.9056439,16.3198574 C13.5509601,17.3729184 11.8487115,18 10,18 C5.581722,18 2,14.418278 2,10 C2,5.581722 5.581722,2 10,2 C14.418278,2 18,5.581722 18,10 C18,11.8487115 17.3729184,13.5509601 16.3198574,14.9056439 Z M10,16 C13.3137085,16 16,13.3137085 16,10 C16,6.6862915 13.3137085,4 10,4 C6.6862915,4 4,6.6862915 4,10 C4,13.3137085 6.6862915,16 10,16 Z\"/></svg>";
