/**
 * @file
 * Initiate ColorPicker.
 */

(function($) {
  'use strict';
  Drupal.behaviors.organigrams = {
    attach: function(context, settings) {

      // Check if the colorpicker plugin is loaded.
      if (jQuery().colorpicker) {
        // Default pointer width.
        var pointer_width = 32;
        // Get the input width.
        var input_width = $('.colorpicker').width();
        if (input_width == 0) {
          input_width = $('input').width();
        }

        // Initialize the colorpicker plugin.
        $('.colorpicker').colorpicker();

        // Overwrite the default styling to place the colorpicker in front of the
        // input field.
        $('.evo-cp-wrap').width(input_width);
        $('.colorpicker').css('float', 'right').css('width', input_width - pointer_width);
        $('.evo-pointer').css('float', 'left');
      }

    }
  };
}(jQuery));
