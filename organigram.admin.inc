<?php
/**
 * @file
 * Defines the administration forms for managing organigrams.
 */

/**
 * List all organigrams form.
 *
 * @param $form
 *   Contains form elements.
 *
 * @return array
 *   Renderable form array.
 */
function organigram_form_overview_organigrams($form) {
  // Retrieve all organigrams.
  $organigrams = organigram_get_organigrams();
  // All form should be rendered as a tree.
  $form['#tree'] = TRUE;
  // Add items container.
  $form['items'] = array();
  // Retrieve organigram count.
  $organigram_count = count($organigrams);

  // Iterate through organigrams.
  foreach ($organigrams as $organigram) {
    // Retrieve the form element entry by reference.
    $organigram_element = &$form['items'][$organigram->oid];
    // Initialize the organigram element.
    $organigram_element = array();

    // Add the organigram as a property of the form element.
    $organigram_element['#organigram'] = $organigram;

    // Add organigram name as a markup element.
    $organigram_element['name'] = array('#markup' => check_plain($organigram->name));

    // Add organigram weight.
    $organigram_element['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for @name', array('@name' => $organigram->name)),
      '#title_display' => 'invisible',
      '#delta' => $organigram_count,
      '#default_value' => $organigram->weight,
    );

    // Add list, view, add, edit and delete links.
    $organigram_element['list'] = array(
      '#type' => 'link',
      '#title' => t('list items'),
      '#href' => "admin/structure/organigram/{$organigram->machine_name}",
    );

    $organigram_element['view'] = array(
      '#type' => 'link',
      '#title' => t('view'),
      '#href' => "organigram/{$organigram->machine_name}",
    );

    $organigram_element['add'] = array(
      '#type' => 'link',
      '#title' => t('add item'),
      '#href' => "admin/structure/organigram/{$organigram->machine_name}/add",
    );

    $organigram_element['edit'] = array(
      '#type' => 'link',
      '#title' => t('edit'),
      '#href' => "admin/structure/organigram/{$organigram->machine_name}/edit",
    );

    $organigram_element['delete'] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => "admin/structure/organigram/{$organigram->machine_name}/delete",
    );
  }

  $form['#empty_text'] = t('No organigrams available. <a href="@link">Add organigram</a>.', array('@link' => url('admin/structure/organigram/add')));

  // Only make this form include a submit button and weight if more than one
  // organigram exists.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Create or edit an organigram form.
 *
 * @param $form
 *   The form being used to edit the node.
 * @param $form_state
 *   The form state array.
 * @param $edit
 *   The object to update.
 *
 * @return mixed
 *   Renderable array containing a form.
 */
function organigram_form_organigram($form, &$form_state, $edit = array()) {
  // During initial form build, add the entity to the form state for use
  // during form building and processing. During a rebuild, use what is in
  // the form state.
  if (!isset($form_state['organigram'])) {
    // Convert the edit variable to an object.
    $organigram = is_object($edit) ? $edit : (object) $edit;
    // Create array with default values.
    $default = array(
      'name' => '',
      'machine_name' => '',
      'description' => '',
      'status' => 1,
      'canvas_width' => 'auto',
      'canvas_height' => 'auto',
      'center' => 0,
      'node_width' => 120,
      'node_height' => 30,
      'top_radius' => 0,
      'bottom_radius' => 0,
      'shadow_offset' => 0,
      'horizontal_space' => 20,
      'vertical_space' => 20,
      'horizontal_offset' => 30,
      'line_color' => '',
      'border_color' => '',
      'background_color' => '',
      'font_color' => '',
      'font_name' => '',
      'font_size' => '',
      'vertical_alignment' => 1,
      'weight' => 0,
    );
    // Iterate through default values.
    foreach ($default as $key => $value) {
      // If the property is missing.
      if (!isset($organigram->{$key})) {
        // Then add the default value.
        $organigram->{$key} = $value;
      }
    }
    // Assign organigram to the form state.
    $form_state['organigram'] = $organigram;
  }
  else {
    // Retrieve the organigram from the form state.
    $organigram = $form_state['organigram'];
  }
  // Add organigram to the form.
  $form['#organigram'] = $form_state['organigram'];

  // Load the extra fields information so set the correct order.
  $extrafields = field_info_extra_fields('organigram', 'organigram', 'form');

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name of the organigram is used as title.'),
    '#default_value' => $organigram->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => isset($extrafields['name']['weight']) ? $extrafields['name']['weight'] : 0,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $organigram->machine_name,
    '#maxlength' => 255,
    '#machine_name' => array(
      'exists' => 'organigram_machine_name_load',
    ),
  );
  $form['old_machine_name'] = array(
    '#type' => 'value',
    '#value' => $organigram->machine_name,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('The description can used as an explanatory text.'),
    '#default_value' => $organigram->description,
    '#weight' => isset($extrafields['#description']['weight']) ? $extrafields['#description']['weight'] : 1,
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#description' => t('Unpublished organigrams will not be displayed on the website.<br />Note that users with the permission to edit this organigram will still be able to see this organigram.'),
    '#default_value' => $organigram->status,
    '#weight' => isset($extrafields['status']['weight']) ? $extrafields['status']['weight'] : 2,
  );

  // Canvas settings.
  $form['canvas'] = array(
    '#type' => 'fieldset',
    '#title' => t('Organigram settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => isset($extrafields['organigram_settings']['weight']) ? $extrafields['organigram_settings']['weight'] : 3,
  );
  $form['canvas']['canvas_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Organigram width'),
    '#description' => t('Specify the width in pixels of the organigram.<br /><br />Other available options:<br /><br /><ul><li><b>auto</b>: Calculate the width by the number of items</li><li><b>parent</b>: Adjust the width to the parent element of the organigram.</li></ul>'),
    '#default_value' => $organigram->canvas_width,
  );
  $form['canvas']['canvas_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Organigram height'),
    '#description' => t('Specify the height in pixels of the organigram. The same options as width are available here.'),
    '#default_value' => $organigram->canvas_height,
  );
  $form['canvas']['center'] = array(
    '#type' => 'checkbox',
    '#title' => t('Center on page'),
    '#description' => t('Aligns the organigram on the center of the page.'),
    '#default_value' => $organigram->center,
  );
  $form['canvas']['line_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Line color'),
    '#description' => t('Specify the connection lines color in this organigram.'),
    '#default_value' => $organigram->line_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );

  // Node settings.
  $form['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Item settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => isset($extrafields['item_settings']['weight']) ? $extrafields['item_settings']['weight'] : 4,
  );
  $form['node']['node_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Item width'),
    '#description' => t('Specify the width in pixels of the items in this organigram.<br>Extra options are <b>auto</b> for automatic width calculation and <b>parent</b> to use the width of its parent container'),
    '#default_value' => $organigram->node_width,
  );
  $form['node']['node_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Item height'),
    '#description' => t('Specify the height in pixels of the items in this organigram.<br>Extra options are <b>auto</b> for automatic height calculation and <b>parent</b> to use the height of its parent container'),
    '#default_value' => $organigram->node_height,
  );
  $form['node']['top_radius'] = array(
    '#type' => 'textfield',
    '#title' => t('Top radius'),
    '#description' => t('Specify the radius of the top corners in pixels of the items in this organigram.'),
    '#default_value' => $organigram->top_radius,
  );
  $form['node']['bottom_radius'] = array(
    '#type' => 'textfield',
    '#title' => t('Bottom radius'),
    '#description' => t('Specify the radius of the bottom corners in pixels of the items in this organigram.'),
    '#default_value' => $organigram->bottom_radius,
  );
  $form['node']['shadow_offset'] = array(
    '#type' => 'textfield',
    '#title' => t('Shadow offset'),
    '#description' => t('Specify the shadow offset in pixels of the items in this organigram.'),
    '#default_value' => $organigram->shadow_offset,
  );
  $form['node']['horizontal_space'] = array(
    '#type' => 'textfield',
    '#title' => t('Horizontal space'),
    '#description' => t('Specify the horizontal space in pixels between the items in this organigram.'),
    '#default_value' => $organigram->horizontal_space,
  );
  $form['node']['vertical_space'] = array(
    '#type' => 'textfield',
    '#title' => t('Vertical space'),
    '#description' => t('Specify the vertical space in pixels between the items in this organigram.'),
    '#default_value' => $organigram->vertical_space,
  );
  $form['node']['horizontal_offset'] = array(
    '#type' => 'textfield',
    '#title' => t('Horizontal offset'),
    '#description' => t('Specify the horizontal offset in pixels between the l and r items in this organigram.'),
    '#default_value' => $organigram->horizontal_offset,
  );
  $form['node']['border_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Border color'),
    '#description' => t('Specify the border color of the items in this organigram.'),
    '#default_value' => $organigram->border_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['node']['background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#description' => t('Specify the background color of the items in this organigram.'),
    '#default_value' => $organigram->background_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );

  // Font settings.
  $form['font'] = array(
    '#type' => 'fieldset',
    '#title' => t('Font settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => isset($extrafields['font_settings']['weight']) ? $extrafields['font_settings']['weight'] : 5,
  );
  $form['font']['font_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Font color'),
    '#description' => t('Specify the font color of the items in this organigram.'),
    '#default_value' => $organigram->font_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['font']['font_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Font name'),
    '#description' => t('Specify the font name of the items in this organigram.'),
    '#default_value' => $organigram->font_name,
  );
  $form['font']['font_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Font size'),
    '#description' => t('Specify the font size in pixels of the items in this organigram.'),
    '#default_value' => $organigram->font_size,
  );
  $form['font']['vertical_alignment'] = array(
    '#type' => 'select',
    '#title' => t('Vertical text alignment'),
    '#description' => t('Specify the vertical text alignment of the items in this organigram.'),
    '#options' => array(
      0 => t('Top'),
      1 => t('Center'),
    ),
    '#default_value' => $organigram->vertical_alignment,
  );

  // Add supported actions for this form.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  // Attach the ColorPicker library if it exists.
  $form['#attached']['libraries_load'][] = array('colorpicker');
  $library = libraries_detect('colorpicker');
  if (!empty($library['installed'])) {
    $form['#attached']['js'][] = drupal_get_path('module', 'organigram') . '/js/colorpicker.js';
  }

  // Add fields to the form.
  field_attach_form('organigram', $organigram, $form, $form_state);

  // Return the generated form.
  return $form;
}

/**
 * Confirm organigram deletion form.
 *
 * @param $form
 *   The form being used to edit the node.
 * @param $form_state
 *   The form state array.
 * @param $organigram
 *   The object to delete.
 *
 * @return mixed
 *   Renderable array containing a form.
 */
function organigram_form_confirm_delete_organigram($form, &$form_state, $organigram) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['oid'] = array('#type' => 'value', '#value' => $organigram->oid);
  // Add organigram to the form.
  $form['#organigram'] = $organigram;
  $form['type'] = array('#type' => 'value', '#value' => 'organigram');
  $form['name'] = array('#type' => 'value', '#value' => $organigram->name);
  return confirm_form(
      $form,
      t('Are you sure you want to delete the organigram %name?', array('%name' => $organigram->name)),
      'admin/structure/organigram',
      t('Deleting an organigram will delete all the organigram items in it. This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );
}

/*
 * Form validation handlers.
 */

/**
 * Form validation handler for 'organigram_form_organigram'.
 */
function organigram_form_organigram_validate($form, &$form_state) {
  // During the deletion there is no 'machine_name' key.
  if (isset($form_state['values']['machine_name'])) {
    // Do not allow machine names to conflict with organigram path arguments.
    $machine_name = $form_state['values']['machine_name'];
    // Invalid machine names which will conflict with the organigram path
    // arguments.
    $disallowed = array('add', 'list');
    // Check if the machine name has on of the disallowed values.
    if (in_array($machine_name, $disallowed)) {
      // Raise form error.
      form_set_error('machine_name', t('The machine-readable name cannot be "add" or "list".'));
    }
  }

  // Allow the entity fields to be validated.
  entity_form_field_validate('organigram', $form, $form_state);
}

/*
 * Form submit handlers.
 */

/**
 * Form submit handler for 'organigram_form_overview_organigrams'.
 */
function organigram_form_overview_organigrams_submit($form, &$form_state) {
  // Check if the form state contains any items.
  if (!empty($form_state['values']['items'])) {
    // Iterate through the form state values.
    foreach ($form_state['values']['items'] as $oid => $organigram) {
      // Retrieve the form element by reference.
      $organigram = &$form['items'][$oid]['#organigram'];
      // Retrieve the form state weight.
      $weight = $form_state['values']['items'][$oid]['weight'];
      // Validate the element name and weight difference.
      if ($organigram->weight != $weight) {
        // Save the new weight.
        $organigram->weight = $weight;
        // Save changes to the DB.
        organigram_save($organigram);
      }
    }
  }
}

/**
 * Form submit handler for 'organigram_form_organigram'.
 */
function organigram_form_organigram_submit($form, &$form_state) {
  // Retrieve the organigram.
  $organigram = $form_state['organigram'];

  // Submit the organigram entity.
  entity_form_submit_build_entity('organigram', $organigram, $form, $form_state);
  // Evaluate the save status.
  switch (organigram_save($organigram)) {
    case SAVED_NEW:
      drupal_set_message(t('Created new organigram %name.', array('%name' => $organigram->name)));
      watchdog('organigram', 'Created new organigram %name.', array('%name' => $organigram->name), WATCHDOG_NOTICE, l(t('edit'), "admin/structure/organigram/{$organigram->machine_name}/edit"));
      break;

    case SAVED_UPDATED:
      drupal_set_message(t('Updated organigram %name.', array('%name' => $organigram->name)));
      watchdog('organigram', 'Updated organigram %name.', array('%name' => $organigram->name), WATCHDOG_NOTICE, l(t('edit'), "admin/structure/organigram/{$organigram->machine_name}/edit"));
      break;
  }
  // Add organigram  information to the form state.
  $form_state['values']['oid'] = $organigram->oid;
  $form_state['oid'] = $organigram->oid;
  // Redirect to the 'admin/structure/organigram' page.
  $form_state['redirect'] = 'admin/structure/organigram';
}

/**
 * Form submit handler for 'organigram_form_confirm_delete_organigram'.
 */
function organigram_form_confirm_delete_organigram_submit($form, &$form_state) {
  // Delete the organigram.
  organigram_delete($form_state['values']['oid']);
  drupal_set_message(t('Deleted organigram %name', array('%name' => $form_state['values']['name'])));
  watchdog('organigram', 'Deleted organigram %name', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/structure/organigram';
  // Clear the page and block caches to avoid stale data.
  cache_clear_all();
}
