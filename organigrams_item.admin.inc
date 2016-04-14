<?php
/**
 * @file
 * Defines the user forms and pages for managing organigrams items.
 */

/**
 * Callback to redirect back to the organigrams item list overview.
 *
 * @param string $organigrams_machine_name
 *   An organigrams machine name.
 */
function organigrams_redirect_to_organigrams_item_overview($organigrams_machine_name) {
  drupal_goto("admin/structure/organigrams/{$organigrams_machine_name}");
}

/**
 * Generates a form overview of all organigrams items.
 *
 * @param array $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 * @param object $organigram
 *   An organigrams entity.
 *
 * @return mixed
 *   Renderable array containing a form.
 */
function organigrams_form_overview_organigrams_items($form, &$form_state, $organigram) {
  $form['#organigram'] = $organigram;
  $form['#tree'] = TRUE;
  // Retrieve the organigram tree.
  $tree = organigrams_get_tree($organigram->oid);
  // Get tree count.
  $tree_item_count = count($tree);
  // Add items container.
  $form['items'] = array();
  // Iterate through the tree.
  foreach ($tree as $tree_item) {
    // Retrieve element by reference.
    $element = &$form['items'][$tree_item->iid];
    // Initialize element.
    $element = array('#organigrams_item' => $tree_item);
    $element['view'] = array('#markup' => filter_xss_admin($tree_item->name));
    $element['iid'] = array('#type' => 'hidden', '#value' => $tree_item->iid);
    $element['parent'] = array('#type' => 'hidden', '#default_value' => $tree_item->parent);
    $element['depth'] = array('#type' => 'hidden', '#default_value' => $tree_item->depth);
    $element['weight'] = array(
      '#type' => 'weight',
      '#delta' => $tree_item_count,
      '#title_display' => 'invisible',
      '#title' => t('Weight for added organigram item'),
      '#default_value' => $tree_item->weight,
    );
    // Only add the operations which the user has access to.
    if (organigrams_user_access('edit', $organigram)) {
      $element['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => "admin/structure/organigrams/{$organigram->machine_name}/item/{$tree_item->iid}",
      );
    }
    else {
      $element['edit'] = array();
    }
    // Only add the operations which the user has access to.
    if (organigrams_user_access('delete', $organigram)) {
      $element['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => "admin/structure/organigrams/{$organigram->machine_name}/item/{$tree_item->iid}/delete",
      );
    }
    else {
      $element['delete'] = array();
    }
  }
  $form['#empty_text'] = t('No organigram items available. <a href="@link">Add organigram item</a>.', array('@link' => url("admin/structure/organigrams/{$organigram->machine_name}/add")));

  // Add the supported actions for this form.
  $form['actions'] = array('#type' => 'actions', '#tree' => TRUE);
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Create or edit form for an organigrams item.
 *
 * @param array $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 * @param array $edit
 *   The object to edit.
 * @param mixed $organigram
 *   The parent organigram entity.
 *
 * @return mixed
 *   Renderable array containing a form.
 */
function organigrams_form_organigrams_item($form, &$form_state, $edit = array(), $organigram = NULL) {
  // During initial form build, add the organigrams item entity to the form
  // state for use during form building and processing. During a rebuild, use
  // what is in the form state.
  if (!isset($form_state['organigrams_item'])) {
    // Convert the edit variable to an object.
    $organigrams_item = is_object($edit) ? $edit : (object) $edit;

    // If the organigram argument is not set and the organigrams item has an
    // oid.
    if (!isset($organigram) && isset($organigrams_item->oid)) {
      // Load the organigram.
      $organigram = organigrams_load($organigrams_item->oid);
    }

    // Create default values array.
    $defaults = array(
      'iid' => NULL,
      'oid' => (isset($organigram) ? $organigram->oid : NULL),
      'name' => '',
      'description' => '',
      'organigrams_machine_name' => (isset($organigram) ? $organigram->machine_name : NULL),
      'position' => 'u',
      'url' => '',
      'border_color' => '',
      'background_color' => '',
      'font_color' => '',
      'bold' => 0,
      'image_url' => '',
      'image_alignment' => '',
      'parent' => 0,
      'weight' => 0,
    );

    // Iterate through defaults.
    foreach ($defaults as $key => $value) {
      // If the property is not set then add the default value.
      if (!isset($organigrams_item->{$key})) {
        $organigrams_item->{$key} = $value;
      }
    }

    // Save the organigrams item to the form state.
    $form_state['organigrams_item'] = $organigrams_item;
  }
  else {
    // Retrieve the organigrams item from the form state.
    $organigrams_item = $form_state['organigrams_item'];
  }

  // Retrieve parent for the current organigrams item.
  $parent = organigrams_item_get_parent($organigrams_item->iid);

  // Build the form.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The content of the organigram item. Use <b>[br]</b> for line breaks.'),
    '#default_value' => $organigrams_item->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  $form['organigrams_machine_name'] = array(
    '#type' => 'value',
    '#value' => (isset($organigrams_item->organigrams_machine_name) ? $organigrams_item->organigrams_machine_name : $organigram->machine_name),
  );

  // Create default options array and merge with suitable parents.
  $parent_relation_options = array(0 => t('<root>')) + organigrams_item_get_suitable_parents_options($organigrams_item->iid, $organigram->oid);
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent item'),
    '#description' => t('The parent item defines the hierarchical place within the organigram for the current item. An organigram item which is connected to the "<root>" item will be at the top of the organigram.'),
    '#options' => $parent_relation_options,
    '#default_value' => (isset($parent) ? $parent->iid : NULL),
    '#weight' => -4,
  );

  $form['position'] = array(
    '#type' => 'select',
    '#title' => t('Position'),
    '#description' => t('The position of the organigram item.'),
    '#options' => array(
      'u' => t('Below the parent'),
      'l' => t('Below the parent, separated on the left side'),
      'r' => t('Below the parent, separated on the right side'),
    ),
    '#default_value' => $organigrams_item->position,
    '#weight' => -3,
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('organigram items are displayed in ascending order by weight.'),
    '#delta' => count($parent_relation_options),
    '#default_value' => $organigrams_item->weight,
    '#weight' => -2,
  );

  // Options.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -1,
  );
  $form['options']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Add a link to this URL to the organigram item.'),
    '#default_value' => $organigrams_item->url,
  );
  $form['options']['border_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Border color'),
    '#description' => t('The border color of the organigram item.'),
    '#default_value' => $organigrams_item->border_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#description' => t('The background color of the organigram item.'),
    '#default_value' => $organigrams_item->background_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['font_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Font color'),
    '#description' => t('The font color of the organigram item.'),
    '#default_value' => $organigrams_item->font_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['bold'] = array(
    '#type' => 'checkbox',
    '#title' => t('Bold'),
    '#description' => t('Emphasize the organigram item with an extra border.'),
    '#default_value' => $organigrams_item->bold,
  );
  // @TODO: Use managed file here.
  $form['options']['image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#description' => t('The image URL of an image to add to the organigram item.'),
    '#default_value' => $organigrams_item->image_url,
  );
  $form['options']['image_alignment'] = array(
    '#type' => 'select',
    '#title' => t('Image alignment'),
    '#description' => t('The alignment of the image in the organigram item.'),
    '#options' => array(
      'lt' => t('Topleft'),
      'ct' => t('Topcenter'),
      'rt' => t('Topright'),
      'lm' => t('Centerleft'),
      'cm' => t('Center'),
      'rm' => t('Centerright'),
      'lb' => t('Bottomleft'),
      'cb' => t('Bottomcenter'),
      'rb' => t('Bottomright'),
    ),
    '#default_value' => $organigrams_item->image_alignment,
  );

  $form['oid'] = array(
    '#type' => 'value',
    '#value' => $organigram->oid,
  );
  $form['iid'] = array(
    '#type' => 'value',
    '#value' => $organigrams_item->iid,
  );
  // Add actions.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  // Redirect the this page.
  $form_state['redirect'] = $_GET['q'];

  // Attach the ColorPicker library if it exists.
  $form['#attached']['libraries_load'][] = array('colorpicker');
  $library = libraries_detect('colorpicker');
  if (!empty($library['installed'])) {
    $form['#attached']['js'][] = drupal_get_path('module', 'organigrams') . '/js/colorpicker.js';
  }

  // Return the generated form.
  return $form;
}

/**
 * Delete organigrams item confirm form.
 *
 * @param array $form
 *   The form being used to edit the node.
 * @param array $form_state
 *   The form state array.
 * @param object $organigrams_item
 *   The object to delete.
 *
 * @return mixed
 *   Renderable array containing a form.
 */
function organigrams_form_confirm_delete_organigrams_item($form, &$form_state, $organigrams_item) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['iid'] = array('#type' => 'value', '#value' => $organigrams_item->iid);
  // Add organigrams item to the form.
  $form['#organigrams_item'] = $organigrams_item;
  $form['organigrams_machine_name'] = array('#type' => 'value', '#value' => $organigrams_item->organigrams_machine_name);
  $form['type'] = array('#type' => 'value', '#value' => 'organigrams_item');
  $form['name'] = array('#type' => 'value', '#value' => $organigrams_item->name);
  return confirm_form(
      $form,
      t('Are you sure you want to delete the organigram item %name?', array('%name' => $organigrams_item->name)),
      "admin/structure/organigrams/{$organigrams_item->organigrams_machine_name}/item/{$organigrams_item->iid}",
      t('Deleting an organigram item will delete all the organigram items underneath it. This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );
}

/*
 * Form validation handlers.
 */

/**
 * Form validation handler for 'organigrams_form_organigrams_item'.
 */
function organigrams_form_organigrams_item_validate($form, &$form_state) {
  // Allow the entity fields to be validated.
  entity_form_field_validate('organigrams_item', $form, $form_state);
}

/*
 * Form submit handlers.
 */

/**
 * Form submit handler for 'organigrams_form_overview_organigrams_items'.
 */
function organigrams_form_overview_organigrams_items_submit($form, &$form_state) {
  // Check the form state contains any organigrams items.
  if (!empty($form_state['values']['items'])) {
    // Iterate through form state items.
    foreach ($form_state['values']['items'] as $iid => $organigrams_item) {
      // Save the changes in the parent and weight.
      db_update('organigrams_item_data')
        ->fields(array('parent' => $organigrams_item['parent'], 'weight' => $organigrams_item['weight']))
        ->condition('iid', $iid, '=')
        ->execute();
    }
  }
  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Form submit handler for 'organigrams_form_organigrams_item'.
 */
function organigrams_form_organigrams_item_submit($form, &$form_state) {
  // Retrieve the organigrams item.
  $organigrams_item = $form_state['organigrams_item'];
  // Submit the entity fields.
  entity_form_submit_build_entity('organigrams_item', $organigrams_item, $form, $form_state);
  // Evaluate the save operation.
  switch (organigrams_item_save($organigrams_item)) {
    case SAVED_NEW:
      drupal_set_message(t('Created new organigram item %name.', array('%name' => $organigrams_item->name)));
      watchdog(
          'organigrams',
          'Created new organigram item %name',
          array(
            '%name' => $organigrams_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "admin/structure/organigrams/item/{$organigrams_item->iid}/edit"));
      break;

    case SAVED_UPDATED:
      drupal_set_message(t('Updated organigram item %name.', array('%name' => $organigrams_item->name)));
      watchdog(
          'organigrams',
          'Updated organigram item %name',
          array(
            '%name' => $organigrams_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "admin/structure/organigrams/item/{$organigrams_item->iid}/edit"));
      // Clear the page and block caches to avoid stale data.
      cache_clear_all();
      break;
  }
  $form_state['values']['iid'] = $organigrams_item->iid;
  $form_state['iid'] = $organigrams_item->iid;
}

/**
 * Form submit handler for 'organigrams_form_confirm_delete_organigrams_item'.
 */
function organigrams_form_confirm_delete_organigrams_item_submit($form, &$form_state) {
  // Delete the organigrams item.
  organigrams_item_delete($form_state['values']['iid']);
  drupal_set_message(t('Deleted organigram item %name', array('%name' => $form_state['values']['name'])));
  watchdog('organigrams', 'Deleted organigram item %name', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = "admin/structure/organigrams/{$form_state['values']['organigrams_machine_name']}";
  // Clear the page and block caches to avoid stale data.
  cache_clear_all();
}
