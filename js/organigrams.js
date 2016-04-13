/**
 * @file
 * Initiate organigrams.
 */

(function ($) {
  Drupal.behaviors.organigrams = {
    attach: function (context, settings) {

      var redrawOnResize = {};

      // Start loading organigrams.
      organigramLoader();

      /**
       * Load all the registered organigrams.
       */
      function organigramLoader() {
        // Are there organigrams that need processing?
        if (organigramSettingExists('organigrams')) {
          // Iterate through the organigram callbacks.
          for (var organigram_id in Drupal.settings.organigrams.organigrams) {
            // Retrieve the callback function.
            var organigram_data = Drupal.settings.organigrams.organigrams[organigram_id];
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

      /**
       * Prepare the DOM model for the javascript organigram chart.
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
       * @param organigram_id
       *    Unique organigram hash.
       *
       * @returns {Array}
       *    Array with node objects.
       */
      function organigramExtractNodes(organigram_id) {
        var i,
          organigram_items,
          nodes = [],
          attributes = [
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
              nodes[index][attributes[i]] = $(li).attr(attributes[i]);
            }
          });
        }

        return nodes;
      }

      /**
       * Load the organigram chart.
       */
      function organigramLoad(organigram_id, organigram_data) {
        // Retrieve the organigram chart container.
        var chart_container = $('#organigram-chart-' + organigram_id + '-canvas');

        // Validate the chart_container.
        if (chart_container.length > 0 && organigram_data['organigram_settings'] != undefined && organigram_data['nodes'] != undefined && organigram_data['nodes'].length > 0) {
          // Define vars.
          var i,
            organigram_width = 'parent',
            organigram_height = 'auto',
            organigram_position = 'left',
            color_properties = {},
            font_properties = {},
            size_properties = {},
            node_properties = {},
            o = new orgChart();

          // Set global organigram settings.
          if (Object.keys(organigram_data['organigram_settings']).length > 0) {
            // Set the organigram width.
            if (organigram_data['organigram_settings']['canvas_width'] != undefined) {
              organigram_width = organigram_data['organigram_settings']['canvas_width'];
            }
            // Set the organigram height.
            if (organigram_data['organigram_settings']['canvas_height'] != undefined) {
              organigram_height = organigram_data['organigram_settings']['canvas_height'];
            }
            // Set the organigram position.
            if (organigram_data['organigram_settings']['center'] == 1) {
              organigram_position = 'center';
            }

            // Add the canvas to the resize watch list.
            if (organigram_width == 'parent') {
              // Set a max width to restore the organigram to its original size.
              if (organigram_data['max_width'] == undefined) {
                organigram_data['max_width'] = chart_container.parent().width();
              }
              redrawOnResize[organigram_id] = organigram_data;
            }

            // Set the color properties.
            color_properties = {
              'border_color': undefined,
              'background_color': undefined,
              'font_color': undefined,
              'line_color': undefined
            };
            color_properties = organigramGetProperties(color_properties, organigram_data);
            o.setColor(color_properties.border_color, color_properties.background_color, color_properties.font_color, color_properties.line_color);

            // Set the font properties.
            font_properties = {
              'font_name': undefined,
              'font_size': undefined,
              'font_color': undefined,
              'vertical_alignment': undefined
            };
            font_properties = organigramGetProperties(font_properties, organigram_data);
            o.setFont(font_properties.font_name, font_properties.font_size, font_properties.font_color, font_properties.vertical_alignment);

            // Set the size properties.
            size_properties = {
              'node_width': undefined,
              'node_height': undefined,
              'horizontal_space': undefined,
              'vertical_space': undefined,
              'horizontal_offset': undefined
            };
            size_properties = organigramGetProperties(size_properties, organigram_data);
            o.setSize(size_properties.node_width, size_properties.node_height, size_properties.horizontal_space, size_properties.vertical_space, size_properties.horizontal_offset);

            // Set the node style properties.
            node_properties = {
              'top_radius': undefined,
              'bottom_radius': undefined,
              'shadow_offset': undefined
            };
            node_properties = organigramGetProperties(node_properties, organigram_data);
            o.setNodeStyle(node_properties.top_radius, node_properties.bottom_radius, node_properties.shadow_offset);
          }

          // Iterate through the nodes.
          for (i in organigram_data['nodes']) {
            o.addNode(
              organigram_data['nodes'][i].id,
              organigram_data['nodes'][i].parent,
              organigram_data['nodes'][i].position,
              organigram_data['nodes'][i].text,
              organigram_data['nodes'][i].bold == 0 ? undefined : 1,
              organigram_data['nodes'][i].url,
              organigram_data['nodes'][i].border_color,
              organigram_data['nodes'][i].background_color,
              organigram_data['nodes'][i].font_color,
              organigram_data['nodes'][i].image_url.length > 0 ? organigram_data['nodes'][i].image_url : undefined,
              organigram_data['nodes'][i].image_alignment
            );
          }

          // Draw the organigram.
          o.drawChart($(chart_container).attr('id'), organigram_width, organigram_height, organigram_position);
        }
      }

      /**
       * Check if the specified organigram setting is present.
       */
      function organigramSettingExists(name) {
        return Drupal.settings.organigrams != null &&
          Drupal.settings.organigrams[name] != null;
      }

      /**
       * Replace properties in the given array with data from the organigram_data.
       *
       * @param properties
       *    Array with keys to look for in the organigram_data array.
       * @param organigram_data
       *    Array with all organigram data.
       *
       * @returns array
       *    The same properties array but with values from the organigram_data array.
       */
      function organigramGetProperties(properties, organigram_data) {
        for (var i in properties) {
          if (organigram_data['organigram_settings'][i] != undefined && organigram_data['organigram_settings'][i].length > 0) {
            properties[i] = organigram_data['organigram_settings'][i];
            // Make sure an integer is an integer.
            if (!isNaN(parseInt(properties[i]))) {
              properties[i] = parseInt(properties[i]);
            }
          }
        }
        return properties;
      }

      $(window).resize(function() {
        // Iterate through all organigrams which need to scale according to its
        // parent.
        for (var id in redrawOnResize) {
          // Retrieve the organigram chart container.
          var chart_container = $('#organigram-chart-' + id + '-canvas');
          // Load its parent width.
          var parent_width = chart_container.parent().width();

          // Update the canvas width and redraw the organigram.
          chart_container.width(parent_width);
          organigramLoad(id, redrawOnResize[id]);
        }
      });

    }
  };
}(jQuery));

/**
 * IE8 fix.
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
