/**
 * @file
 * Initiate organigrams.
 */

(function ($) {
  'use strict';
  Drupal.behaviors.organigrams = {
    attach: function (context, settings) {

      var redrawOnResize = {};

      /**
       * IE8 fix.
       *
       * @param {Object} obj
       *   Object to get the keys from.
       *
       * @return {Array}
       *   Array with object keys.
       */
      if (!Object.keys) {
        Object.keys = function (obj) {
          var keys = [];

          for (var i in obj) {
            if (obj.hasOwnProperty(i)) {
              keys.push(i);
            }
          }

          return keys;
        };
      }

      // Start loading organigrams.
      organigramLoader();

      /**
       * Load all the registered organigrams.
       */
      function organigramLoader() {
        var organigram_id;
        var organigram_data;

        // Are there organigrams that need processing?
        if (organigramSettingExists('organigrams')) {
          // Iterate through the organigram callbacks.
          for (organigram_id in Drupal.settings.organigrams.organigrams) {
            // Guard-for-in.
            if (Drupal.settings.organigrams.organigrams.hasOwnProperty(organigram_id)) {
              // Retrieve the callback function.
              organigram_data = Drupal.settings.organigrams.organigrams[organigram_id];
              // Loading the organigram chart happens in three phases.
              // 1. Hide content and perform additional preload logic.
              organigramPreLoad(organigram_id);
              // 2. Extract node data from the organigram list items.
              organigram_data['nodes'] = organigramExtractNodes(organigram_id);
              // 3. Load the organigram chart.
              organigramLoad(organigram_id, organigram_data);
            }
          }
        }
      }

      /**
       * Prepare the DOM model for the javascript organigram chart.
       *
       * @param {string} organigram_id - Unique organigram hash.
       */
      function organigramPreLoad(organigram_id) {
        // Retrieve the organigram items container.
        var organigram_items_container = $('div.organigram-items#organigram-items-' + organigram_id);
        // Check if the organigram items container exists.
        if (organigram_items_container != null) {
          // Hide organigram content container.
          organigram_items_container.hide();
          // Insert the container for the Google Chart which will represent the
          // organigram.
          $('<canvas id="organigram-chart-' + organigram_id + '-canvas" class="organigram-chart"></canvas>').insertBefore(organigram_items_container);
        }
      }

      /**
       * Extract node data from organigram list items.
       *
       * @param {string} organigram_id - Unique organigram hash.
       *
       * @return {Array} - Array with node objects.
       */
      function organigramExtractNodes(organigram_id) {
        var i;
        var organigram_items;
        var nodes = [];
        var attributes = [
          'id',
          'parent',
          'position',
          'text',
          'bold',
          'url',
          'border_color',
          'background_color',
          'font_color',
          'image_url',
          'image_alignment'
        ];

        // Retrieve all li items in the organigram items container.
        organigram_items = $('div.organigram-items#organigram-items-' + organigram_id).find('li');

        // Check if there are any items.
        if (organigram_items.length > 0) {
          // Iterate through the li items.
          organigram_items.each(function (index, li) {
            // Add a node with all attributes specified in the attributes array.
            nodes[index] = {};
            for (i in attributes) {
              if (attributes.hasOwnProperty(i)) {
                nodes[index][attributes[i]] = $(li).attr(attributes[i]);
              }
            }
          });
        }

        return nodes;
      }

      /**
       * Load the organigram chart.
       *
       * @param {string} organigram_id - Unique organigram hash.
       * @param {Array} organigram_data - Contains organigram settings and nodes.
       */
      function organigramLoad(organigram_id, organigram_data) {
        // Retrieve the organigram chart container.
        var chart_container = $('#organigram-chart-' + organigram_id + '-canvas');

        // Validate the chart_container.
        if (chart_container.length > 0 && typeof organigram_data['organigram_settings'] !== 'undefined' && typeof organigram_data['nodes'] !== 'undefined' && organigram_data['nodes'].length > 0) {
          // Define vars.
          var i;
          var organigram_width = 'parent';
          var organigram_height = 'auto';
          var organigram_position = 'left';
          var color_property_keys = [];
          var color_properties = {};
          var font_property_keys = [];
          var font_properties = {};
          var size_property_keys = [];
          var size_properties = {};
          var node_property_keys = [];
          var node_properties = {};
          var o = new orgChart();

          // Set global organigram settings.
          if (Object.keys(organigram_data['organigram_settings']).length > 0) {
            // Set the organigram width.
            if (typeof organigram_data['organigram_settings']['canvas_width'] !== 'undefined') {
              organigram_width = organigram_data['organigram_settings']['canvas_width'];
            }
            // Set the organigram height.
            if (typeof organigram_data['organigram_settings']['canvas_height'] !== 'undefined') {
              organigram_height = organigram_data['organigram_settings']['canvas_height'];
            }
            // Set the organigram position.
            if (organigram_data['organigram_settings']['center'] === 1) {
              organigram_position = 'center';
            }

            // Add the canvas to the resize watch list.
            if (organigram_width === 'parent') {
              // Set a max width to restore the organigram to its original size.
              if (typeof organigram_data['max_width'] === 'undefined') {
                organigram_data['max_width'] = chart_container.parent().width();
              }
              redrawOnResize[organigram_id] = organigram_data;
            }

            // Set the color properties.
            color_property_keys = [
              'border_color',
              'background_color',
              'font_color',
              'line_color'
            ];
            color_properties = organigramGetProperties(color_property_keys, organigram_data);
            o.setColor(color_properties.border_color, color_properties.background_color, color_properties.font_color, color_properties.line_color);

            // Set the font properties.
            font_property_keys = [
              'font_name',
              'font_size',
              'font_color',
              'vertical_alignment'
            ];
            font_properties = organigramGetProperties(font_property_keys, organigram_data);
            o.setFont(font_properties.font_name, font_properties.font_size, font_properties.font_color, font_properties.vertical_alignment);

            // Set the size properties.
            size_property_keys = [
              'node_width',
              'node_height',
              'horizontal_space',
              'vertical_space',
              'horizontal_offset'
            ];
            size_properties = organigramGetProperties(size_property_keys, organigram_data);
            o.setSize(size_properties.node_width, size_properties.node_height, size_properties.horizontal_space, size_properties.vertical_space, size_properties.horizontal_offset);

            // Set the node style properties.
            node_property_keys = [
              'top_radius',
              'bottom_radius',
              'shadow_offset'
            ];
            node_properties = organigramGetProperties(node_property_keys, organigram_data);
            o.setNodeStyle(node_properties.top_radius, node_properties.bottom_radius, node_properties.shadow_offset);
          }

          // Iterate through the nodes.
          for (i in organigram_data['nodes']) {
            if (organigram_data['nodes'].hasOwnProperty(i)) {
              o.addNode(
                organigram_data['nodes'][i].id,
                organigram_data['nodes'][i].parent,
                organigram_data['nodes'][i].position,
                organigram_data['nodes'][i].text,
                organigram_data['nodes'][i].bold,
                organigram_data['nodes'][i].url,
                organigram_data['nodes'][i].border_color,
                organigram_data['nodes'][i].background_color,
                organigram_data['nodes'][i].font_color,
                organigram_data['nodes'][i].image_url.length > 0 ? organigram_data['nodes'][i].image_url : '',
                organigram_data['nodes'][i].image_alignment
              );
            }
          }

          // Draw the organigram.
          o.drawChart($(chart_container).attr('id'), organigram_width, organigram_height, organigram_position);
        }
      }

      /**
       * Check if the specified organigram setting is present.
       *
       * @param {string} name - The name of an organigram.
       *
       * @return {boolean} - Indicating if the organigram exists.
       */
      function organigramSettingExists(name) {
        return Drupal.settings.organigrams !== null &&
          Drupal.settings.organigrams[name] !== null;
      }

      /**
       * Replace properties in the given array with data from the organigram_data.
       *
       * @param {Array} property_keys - Array with keys to look for in the organigram_data array.
       * @param {Array} organigram_data - Array with all organigram data.
       *
       * @return {Object} - An object with the properties set in the property_keys.
       */
      function organigramGetProperties(property_keys, organigram_data) {
        var properties = {};

        for (var i in property_keys) {
          if (!property_keys.hasOwnProperty(i)) {
            continue;
          }

          if (typeof organigram_data['organigram_settings'][property_keys[i]] !== 'undefined' && organigram_data['organigram_settings'][property_keys[i]].length > 0) {
            properties[property_keys[i]] = organigram_data['organigram_settings'][property_keys[i]];
            // Make sure an integer is an integer.
            if (!isNaN(parseInt(properties[property_keys[i]]))) {
              properties[property_keys[i]] = parseInt(properties[property_keys[i]]);
            }
          }
        }
        return properties;
      }

      $(window).resize(function () {
        // Iterate through all organigrams which need to scale according to its
        // parent.
        for (var id in redrawOnResize) {
          if (redrawOnResize.hasOwnProperty(id)) {
            // Retrieve the organigram chart container.
            var chart_container = $('#organigram-chart-' + id + '-canvas');
            // Load its parent width.
            var parent_width = chart_container.parent().width();

            // Update the canvas width and redraw the organigram.
            chart_container.width(parent_width);
            organigramLoad(id, redrawOnResize[id]);
          }
        }
      });

    }
  };
}(jQuery));
