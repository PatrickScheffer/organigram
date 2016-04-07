
jQuery(document).ready(function() {
  organigramLoader();
});

/**
 * Load all the registered organigrams.
 */
function organigramLoader() {
  // Are there organigrams that need processing?
  if(organigramSettingExists('organigrams')) {
    // Iterate through the organigram callbacks.
    for(var organigram_id in Drupal.settings.organigram.organigrams) {
      // Retrieve the callback function.
      var table_data = Drupal.settings.organigram.organigrams[organigram_id];
      // Loading the organigram chart happens in two phases.
      // 1. Hide content and perform additional preload logic.
      organigramPreLoad(organigram_id);
      // 2. Load the organigram chart.
      organigramLoad(organigram_id, table_data);
    }
  }
}

/**
 * Prepare the DOM model for the javascript organigram chart.
 */
function organigramPreLoad(organigram_id) {
  // Retrieve the organigram items container.
  var organigram_items_container = jQuery('div.organigram-items#organigram-items-' + organigram_id);
  // Check if the organigram items container exists.
  if (organigram_items_container != null) {
    // Hide organigram content container.
    organigram_items_container.hide();
    // Insert the container for the Google Chart which will represent the
    // organigram.
    jQuery('<canvas id="organigram-chart-' + organigram_id + '-canvas" class="organigram-chart"></canvas>').insertBefore(organigram_items_container);
  }
}

/**
 * Load the organigram chart.
 */
function organigramLoad(organigram_id, table_data) {
  // Retrieve the organigram chart container.
  var chart_container = jQuery('#organigram-chart-' + organigram_id + '-canvas');

  // Validate the chart_container.
  if (chart_container.length > 0 && table_data['organigram_settings'] != undefined && table_data['nodes'] != undefined && table_data['nodes'].length > 0) {
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
    if (Object.keys(table_data['organigram_settings']).length > 0) {
      // Set the organigram width.
      if (table_data['organigram_settings']['canvas_width'] != undefined) {
        organigram_width = table_data['organigram_settings']['canvas_width'];
      }
      // Set the organigram height.
      if (table_data['organigram_settings']['canvas_height'] != undefined) {
        organigram_height = table_data['organigram_settings']['canvas_height'];
      }
      // Set the organigram position.
      if (table_data['organigram_settings']['center'] == 1) {
        organigram_position = 'center';
      }

      // Set the color properties.
      color_properties = {
        'border_color': undefined,
        'background_color': undefined,
        'font_color': undefined,
        'line_color': undefined
      };
      color_properties = organigramGetProperties(color_properties, table_data);
      o.setColor(color_properties.border_color, color_properties.background_color, color_properties.font_color, color_properties.line_color);

      // Set the font properties.
      font_properties = {
        'font_name': undefined,
        'font_size': undefined,
        'font_color': undefined,
        'vertical_alignment': undefined
      };
      font_properties = organigramGetProperties(font_properties, table_data);
      o.setFont(font_properties.font_name, font_properties.font_size, font_properties.font_color, font_properties.vertical_alignment);

      // Set the size properties.
      size_properties = {
        'node_width': undefined,
        'node_height': undefined,
        'horizontal_space': undefined,
        'vertical_space': undefined,
        'horizontal_offset': undefined
      };
      size_properties = organigramGetProperties(size_properties, table_data);
      o.setSize(size_properties.node_width, size_properties.node_height, size_properties.horizontal_space, size_properties.vertical_space, size_properties.horizontal_offset);

      // Set the node style properties.
      node_properties = {
        'top_radius': undefined,
        'bottom_radius': undefined,
        'shadow_offset': undefined
      };
      node_properties = organigramGetProperties(node_properties, table_data);
      o.setNodeStyle(node_properties.top_radius, node_properties.bottom_radius, node_properties.shadow_offset);
    }

    // Iterate through the nodes.
    for (i in table_data['nodes']) {
      o.addNode(
        table_data['nodes'][i].id,
        table_data['nodes'][i].parent,
        table_data['nodes'][i].position,
        table_data['nodes'][i].text,
        table_data['nodes'][i].bold == 0 ? undefined : 1,
        table_data['nodes'][i].url,
        table_data['nodes'][i].border_color,
        table_data['nodes'][i].background_color,
        table_data['nodes'][i].font_color,
        table_data['nodes'][i].image_url.length > 0 ? table_data['nodes'][i].image_url : undefined,
        table_data['nodes'][i].image_alignment
      );
    }

    // Draw the organigram.
    o.drawChart(jQuery(chart_container).attr('id'), organigram_width, organigram_height, organigram_position);
  }
}

/**
 * Check if the specified organigram setting is present.
 */
function organigramSettingExists(name) {
  return Drupal.settings.organigram != null &&
         Drupal.settings.organigram[name] != null;
}

/**
 * Replace properties in the given array with data from the table_data.
 *
 * @param properties
 *    Array with keys to look for in the table_data array.
 * @param table_data
 *    Array with all organigram data.
 *
 * @returns array
 *    The same properties array but with values from the table_data array.
 */
function organigramGetProperties(properties, table_data) {
  for (var i in properties) {
    if (table_data['organigram_settings'][i] != undefined && table_data['organigram_settings'][i].length > 0) {
      properties[i] = table_data['organigram_settings'][i];
      // Make sure an integer is an integer.
      if (!isNaN(parseInt(properties[i]))) {
        properties[i] = parseInt(properties[i]);
      }
    }
  }
  return properties;
}

/**
 * IE8 fix.
 */
if (!Object.keys) {
  Object.keys = function(obj) {
    var keys = [];

    for (var i in obj) {
      if (obj.hasOwnProperty(i)) {
        keys.push(i);
      }
    }

    return keys;
  };
}
