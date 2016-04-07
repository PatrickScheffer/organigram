Based on the Organigram module by Frederic Hennequin (https://www.drupal.org/sandbox/daeron/1603000).

Browsersupport
--------------
Organigram is tested in the following browsers:

- Google Chrome version 43.0.2357.124
- Mozilla Firefox version 40.0.3
- Microsoft Internet Explorer > version 7 (ExplorerCanvas is required for IE < 9)

IE < 9 support
--------------
Requires ExplorerCanvas (excanvas) for IE < 9 support.

Steps:
1. Download here: https://github.com/arv/ExplorerCanvas/archive/aa989ea9d9bac748638f7c66b0fc88e619715da6.zip
2. Place the contents of this zip in your libraries folder. See the readme of
   the libraries module for more information about this.
3. Rename the extracted folder to **excanvas** so the path to the javascript
   becomes `libraries/excanvas/excanvas.js`.

ColorPicker
-----------
Organigram offers support for the jQuery ColorPicker plugin by Evoluteur.

Steps to enable this plugin:
1. Download here: https://github.com/evoluteur/colorpicker/archive/233e15e4368b0d1eb30f8036a1f5970f09e8ed9e.zip
2. Place the contents of this zip in your libraries folder. See the readme of
   the libraries module for more information about this.
3. Rename the extracted folder to **colorpicker** so the path to the javascript
   becomes `libraries/colorpicker/js/evol-colorpicker.min.js` and the path to
   the css becomes `libraries/colorpicker/css/evol-colorpicker.min.css`.