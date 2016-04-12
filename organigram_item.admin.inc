<?php
/**
 * @file
 * Defines the user forms and pages for managing organigram items.
 */

/**
 * Callback to redirect back to the organigram item list overview.
 *
 * @param string $organigram_machine_name
 *   An organigram machine name.
 */
function organigram_redirect_to_organigram_item_overview($organigram_machine_name) {
  drupal_goto("admin/structure/organigram/{$organigram_machine_name}");
}

/*
 * Form generator functions.
 */

/**
 * Form generator function for the page 'organigram/%organigram_machine_name'.
 */
function organigram_form_overview_organigram_items($form, &$form_state, $organigram) {
  $form['#organigram'] = $organigram;
  $form['#tree'] = TRUE;
  // Retrieve the organigram tree.
  $tree = organigram_get_tree($organigram->oid);
  // Get tree count.
  $tree_item_count = count($tree);
  // Add items container.
  $form['items'] = array();
  // Iterate through the tree.
  foreach ($tree as $tree_item) {
    // Retrieve element by reference.
    $element = &$form['items'][$tree_item->iid];
    // Initialize element.
    $element = array('#organigram_item' => $tree_item);
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
    if (organigram_user_access('edit', $organigram)) {
      $element['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => "admin/structure/organigram/{$organigram->machine_name}/item/{$tree_item->iid}",
      );
    }
    else {
      $element['edit'] = array();
    }
    // Only add the operations which the user has access to.
    if (organigram_user_access('delete', $organigram)) {
      $element['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => "admin/structure/organigram/{$organigram->machine_name}/item/{$tree_item->iid}/delete",
      );
    }
    else {
      $element['delete'] = array();
    }
  }
  $form['#empty_text'] = t('No organigram items available. <a href="@link">Add organigram item</a>.', array('@link' => url("admin/structure/organigram/{$organigram->machine_name}/add")));

  // Add the supported actions for this form.
  $form['actions'] = array('#type' => 'actions', '#tree' => TRUE);
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Create or edit form for an organigram item.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>admin/structure/organigram/%organigram_machine_name/add</li>
 *   <li>admin/structure/organigram/%organigram_machine_name/item/%organigram_item</li>
 * </ul>
 */
function organigram_form_organigram_item($form, &$form_state, $edit = array(), $organigram = NULL) {
  // During initial form build, add the organigram item entity to the form state
  // for use during form building and processing. During a rebuild, use what is
  // in the form state.
  if (!isset($form_state['organigram_item'])) {
    // Convert the edit variable to an object.
    $organigram_item = is_object($edit) ? $edit : (object) $edit;

    // If the organigram argument is not set and the organigram item has an oid.
    if (!isset($organigram) && isset($organigram_item->oid)) {
      // Load the organigram.
      $organigram = organigram_load($organigram_item->oid);
    }

    // Create default values array.
    $defaults = array(
      'iid' => NULL,
      'oid' => (isset($organigram) ? $organigram->oid : NULL),
      'name' => '',
      'description' => '',
      'organigram_machine_name' => (isset($organigram) ? $organigram->machine_name : NULL),
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
      if (!isset($organigram_item->{$key})) {
        $organigram_item->{$key} = $value;
      }
    }

    // Save the organigram item to the form state.
    $form_state['organigram_item'] = $organigram_item;
  }
  else {
    // Retrieve the organigram item from the form state.
    $organigram_item = $form_state['organigram_item'];
  }

  // Retrieve parent for the current organigram item.
  $parent = organigram_item_get_parent($organigram_item->iid);

  // Build the form.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The content of the organigram item. Use <b>[br]</b> for line breaks.'),
    '#default_value' => $organigram_item->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  $form['organigram_machine_name'] = array(
    '#type' => 'value',
    '#value' => (isset($organigram_item->organigram_machine_name) ? $organigram_item->organigram_machine_name : $organigram->machine_name),
  );

  // Create default options array and merge with suitable parents.
  $parent_relation_options = array(0 => '<' . t('root') . '>') + organigram_item_get_suitable_parents_options($organigram_item->iid, $organigram->oid);
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
    '#default_value' => $organigram_item->position,
    '#weight' => -3,
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('Organigram items are displayed in ascending order by weight.'),
    '#delta' => count($parent_relation_options),
    '#default_value' => $organigram_item->weight,
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
    '#default_value' => $organigram_item->url,
  );
  $form['options']['border_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Border color'),
    '#description' => t('The border color of the organigram item.'),
    '#default_value' => $organigram_item->border_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#description' => t('The background color of the organigram item.'),
    '#default_value' => $organigram_item->background_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['font_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Font color'),
    '#description' => t('The font color of the organigram item.'),
    '#default_value' => $organigram_item->font_color,
    '#attributes' => array(
      'class' => array('colorpicker'),
    ),
  );
  $form['options']['bold'] = array(
    '#type' => 'checkbox',
    '#title' => t('Bold'),
    '#description' => t('Emphasize the organigram item with an extra border.'),
    '#default_value' => $organigram_item->bold,
  );
  // @TODO: Use managed file here.
  $form['options']['image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#description' => t('The image URL of an image to add to the organigram item.'),
    '#default_value' => $organigram_item->image_url,
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
    '#default_value' => $organigram_item->image_alignment,
  );

  $form['oid'] = array(
    '#type' => 'value',
    '#value' => $organigram->oid,
  );
  $form['iid'] = array(
    '#type' => 'value',
    '#value' => $organigram_item->iid,
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
    $form['#attached']['js'][] = drupal_get_path('module', 'organigram') . '/js/colorpicker.js';
  }

  // Return the generated form.
  return $form;
}

/**
 * Delete organigram item confirm form.
 *
 * Used by the following menu items:
 * <ul>
 *   <li>organigram/%organigram_machine_name/item/%organigram_item/delete</li>
 * </ul>
 */
function organigram_form_confirm_delete_organigram_item($form, &$form_state, $organigram_item) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['iid'] = array('#type' => 'value', '#value' => $organigram_item->iid);
  // Add organigram to the form.
  $form['#organigram_item'] = $organigram_item;
  $form['organigram_machine_name'] = array('#type' => 'value', '#value' => $organigram_item->organigram_machine_name);
  $form['type'] = array('#type' => 'value', '#value' => 'organigram_item');
  $form['name'] = array('#type' => 'value', '#value' => $organigram_item->name);
  return confirm_form(
      $form,
      t('Are you sure you want to delete the organigram item %name?', array('%name' => $organigram_item->name)),
      "admin/structure/organigram/{$organigram_item->organigram_machine_name}/item/{$organigram_item->iid}",
      t('Deleting an organigram item will delete all the organigram items underneath it. This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
  );
}

/*
 * Form validation handlers.
 */

/**
 * Form validation handler for 'organigram_form_organigram_item'.
 */
function organigram_form_organigram_item_validate($form, &$form_state) {
  // Allow the entity fields to be validated.
  entity_form_field_validate('organigram_item', $form, $form_state);
}

/*
 * Form submit handlers.
 */

/**
 * Form submit handler for 'organigram_form_overview_organigram_items'.
 */
function organigram_form_overview_organigram_items_submit($form, &$form_state) {
  // Check the form state contains any organigram items.
  if (!empty($form_state['values']['items'])) {
    // Iterate through form state items.
    foreach ($form_state['values']['items'] as $iid => $organigram_item) {
      // Save the changes in the parent and weight.
      db_update('organigram_item_data')
        ->fields(array('parent' => $organigram_item['parent'], 'weight' => $organigram_item['weight']))
        ->condition('iid', $iid, '=')
        ->execute();
    }
  }
  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Form submit handler for 'organigram_form_organigram_item'.
 */
function organigram_form_organigram_item_submit($form, &$form_state) {
  // Retrieve the organigram item.
  $organigram_item = $form_state['organigram_item'];
  // Submit the entity fields.
  entity_form_submit_build_entity('organigram_item', $organigram_item, $form, $form_state);
  // Evaluate the save operation.
  switch (organigram_item_save($organigram_item)) {
    case SAVED_NEW:
      drupal_set_message(t('Created new organigram item %name.', array('%name' => $organigram_item->name)));
      watchdog(
          'organigram',
          'Created new organigram item %name',
          array(
            '%name' => $organigram_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "admin/structure/organigram/item/{$organigram_item->iid}/edit"));
      break;

    case SAVED_UPDATED:
      drupal_set_message(t('Updated organigram item %name.', array('%name' => $organigram_item->name)));
      watchdog(
          'organigram',
          'Updated organigram item %name',
          array(
            '%name' => $organigram_item->name,
          ),
          WATCHDOG_NOTICE,
          l(t('edit'), "admin/structure/organigram/item/{$organigram_item->iid}/edit"));
      // Clear the page and block caches to avoid stale data.
      cache_clear_all();
      break;
  }
  $form_state['values']['iid'] = $organigram_item->iid;
  $form_state['iid'] = $organigram_item->iid;
}

/**
 * Form submit handler for 'organigram_form_confirm_delete_organigram_item'.
 */
function organigram_form_confirm_delete_organigram_item_submit($form, &$form_state) {
  // Delete the organigram.
  organigram_item_delete($form_state['values']['iid']);
  drupal_set_message(t('Deleted organigram item %name', array('%name' => $form_state['values']['name'])));
  watchdog('organigram', 'Deleted organigram item %name', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  $form_state['redirect'] = "admin/structure/organigram/{$form_state['values']['organigram_machine_name']}";
  // Clear the page and block caches to avoid stale data.
  cache_clear_all();
}
