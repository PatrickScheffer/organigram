CONTENTS OF THIS FILE
----------------------

 * Introduction
 * Requirements
 * Plugins
 * Installation
 * Configuration
 * Browser support
 * References


INTRODUCTION
------------

Organigrams provides the ability to easily create and manage organization
charts, also knows as organigrams.

You can create organigram bundles (with own fields and display) and add
organigram items to that bundles. These items contain a static set of fields and
are not fieldable. An organigram will be available as dedicated page
(organigram/%organigram_machine_name) and as a block.

Organigrams are highly customizable. Almost every aspect can be modified,
from border and background colors to horizontal spacing between items.
In addition, most of these settings can be overridden per item.

An organigram is drawn in an HTML5 canvas and is supported by all major
browsers. See the section 'BROWSER SUPPORT' section for more information.

One of Organigrams' key features is responsiveness. When an organigram has a
fixed width it will divide its children over multiple rows to take the given
width into account.
When the width of an organigram is set to match the width of its parent element,
the organigram will redraw itself on window resize and rearrange its children if
necessary (provided the parent element has a dynamic width).

In short, organigrams are:

 * Highly customizable
 * Easy to manage for editors
 * Responsive
 * Supported by all major browsers


REQUIREMENTS
------------

This module requires the following modules:

 * Entity (https://www.drupal.org/project/entity)
   Organigrams and organigram items are entities so the entity API is needed.

 * Libraries (https://www.drupal.org/project/libraries)
   Needed to use external plugins.


PLUGINS
-------

 * ExplorerCanvas (excanvas)
   Provides HTML5 canvas support for Internet Explorer < 9.

   Steps:
   1. Download here: https://github.com/arv/ExplorerCanvas/archive/aa989ea9d9bac748638f7c66b0fc88e619715da6.zip
   2. Place the contents of this zip in your libraries folder. See the readme of
      the libraries module for more information about this.
   3. Rename the extracted folder to 'excanvas' so the path to the javascript
      becomes 'libraries/excanvas/excanvas.js'.

 * ColorPicker by Evoluteur
   Adds a color picker to fields like border color and background color.

   Steps to enable this plugin:
   1. Download here: https://github.com/evoluteur/colorpicker/archive/233e15e4368b0d1eb30f8036a1f5970f09e8ed9e.zip
   2. Place the contents of this zip in your libraries folder. See the readme of
      the libraries module for more information about this.
   3. Rename the extracted folder to 'colorpicker' so the path to the
      javascript becomes 'libraries/colorpicker/js/evol-colorpicker.min.js' and
      the path to the css becomes
      'libraries/colorpicker/css/evol-colorpicker.min.css'.


INSTALLATION
------------

Install as you would normally install a contributed Drupal module. See:
https://drupal.org/documentation/install/modules-themes/modules-7
for further information.

To enhance this module, several plugins are supported. See the 'PLUGINS'
section for more information.


CONFIGURATION
-------------

 * Create organigrams in Administration >> Structure >> Organigram.

 * Configure user permissions in Administration >> People >> Permissions:

   - Manage organigrams
     Administer all organigrams and items.

   - Edit organigram %organigram
     Edit the organigram and its items for a specific organigram.

   - Delete organigram %organigram
     Delete organigram items from a specific organigram.


BROWSER SUPPORT
---------------

Organigrams is tested in the following browsers:

 * Google Chrome version 43.0.2357.124
 * Mozilla Firefox version 40.0.3
 * Microsoft Internet Explorer > version 7 (ExplorerCanvas is required for
   IE < 9, see the 'PLUGINS' section for more information)


REFERENCES
----------

 * Based on the Organigram module by Frederic Hennequin, see:
   https://www.drupal.org/sandbox/daeron/1603000

 * Extended with an improved version of the orgchart library by J. van Loenen,
   see: https://jvloenen.home.xs4all.nl/orgchart/sample.htm)
