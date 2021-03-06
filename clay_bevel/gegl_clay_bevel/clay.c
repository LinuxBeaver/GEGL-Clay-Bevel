/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2022 Beaver (GEGL clay bevel)
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES




enum_start (gegl_emboss_typex)
  enum_value (GEGL_EMBOSS_TYPE_EMBOSSx,  "embossx",  N_("Embossx"))
  enum_value (GEGL_EMBOSS_TYPE_BUMPMAPx, "bumpmapx", N_("Bumpmapx (preserve original colors)"))
enum_end (GeglEmbossTypex)

property_enum (type, _("Emboss Type"),
               GeglEmbossTypex, gegl_emboss_typex, GEGL_EMBOSS_TYPE_BUMPMAPx)
    description(_("Rendering type"))

property_double (azimuth, _("Azimuth"), 70.0)
    description (_("Light angle (degrees)"))
    value_range (30, 90)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")

property_double (elevation, _("Elevation"), 53.0)
    description (_("Elevation angle (degrees)"))
    value_range (25, 90)
    ui_meta ("unit", "degree")

property_int (depth, _("Depth (makes darker)"), 26)
    description (_("Filter width"))
    value_range (1, 100)



property_int  (size, _("Internal Median Blur Radius"), 9)
  value_range (-10, 10)
  ui_range    (-10, 10)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))

property_double  (percentile, _("Internal Median Blur Percentile"), 61)
  value_range (20, 80)
  description (_("Neighborhood color percentile"))

property_double  (alpha_percentile, _("Internal Median Blur Alpha percentile"), 69)
  value_range (17, 100)
  description (_("Neighborhood alpha percentile"))



property_double (gaus, _("Internal Gaussian"), 3)
   description (_("Standard deviation for the horizontal axis"))
   value_range (0.0, 5.0)

property_double (scale, _("Desaturate"), 1.0)
    description(_("Scale, strength of effect"))
    value_range (0.0, 1.0)
    ui_range (0.0, 1.0)

property_double (lightness, _("Lightness"), 0.0)
   description  (_("Lightness adjustment"))
   value_range  (-18.0, 18.0)


property_color  (value, _("Recolor (works only on grayscale image or white text)"), "#ffffff")

property_file_path(src, _("Image file Overlay (works only on grayscale image or white text)"), "")
    description (_("Source image file path (png, jpg, raw, svg, bmp, tif, ...)"))



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     clay
#define GEGL_OP_C_SOURCE clay.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *mcol, *gray, *graph1, *emboss, *median, *median2, *gaussian, *median3, *graph2, *lightness, *imagefileoverlay, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  mcol    = gegl_node_new_child (gegl,
                                  "operation", "gegl:mcol",
                                  NULL);

  gaussian    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);


  median2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  median3    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

  emboss    = gegl_node_new_child (gegl,
                                  "operation", "gegl:emboss",
                                  NULL);

  graph1    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzmediangraph",
                                  NULL);

  graph2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzopacity",
                                  NULL);

  gray    = gegl_node_new_child (gegl,
                                  "operation", "gegl:saturation",
                                  NULL);

  imagefileoverlay    = gegl_node_new_child (gegl,
                                  "operation", "gegl:zzmlayer",
                                  NULL);

  lightness    = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma",
                                  NULL);
 
 
 




  gegl_operation_meta_redirect (operation, "size", median, "radius");

  gegl_operation_meta_redirect (operation, "size2", median2, "radius");


  gegl_operation_meta_redirect (operation, "size3", median3, "radius");

  gegl_operation_meta_redirect (operation, "gaus", gaussian, "std-dev-x");

  gegl_operation_meta_redirect (operation, "gaus", gaussian, "std-dev-y");

  gegl_operation_meta_redirect (operation, "azimuth", emboss, "azimuth");

  gegl_operation_meta_redirect (operation, "elevation", emboss, "elevation");

  gegl_operation_meta_redirect (operation, "depth", emboss, "depth");

  gegl_operation_meta_redirect (operation, "type", emboss, "type");

  gegl_operation_meta_redirect (operation, "percentile", median, "percentile");

  gegl_operation_meta_redirect (operation, "alpha-percentile", median, "alpha-percentile");

  gegl_operation_meta_redirect (operation, "value", mcol, "value");

  gegl_operation_meta_redirect (operation, "scale", gray, "scale");

  gegl_operation_meta_redirect (operation, "src", imagefileoverlay, "src");

  gegl_operation_meta_redirect (operation, "lightness", lightness, "lightness");











  gegl_node_link_many (input, graph1, emboss, median, median2, gaussian, median3, graph2, gray, lightness, mcol, imagefileoverlay, output, NULL);




}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:clay",
    "title",       _("Clay Bevel"),
    "categories",  "Aristic",
    "reference-hash", "33do6a1h24fk10fjf25sb2ac",
    "description", _("Clay Bevel Text styling using GEGL. This does not work on small text.)  "
                     ""),
    NULL);
}

#endif
