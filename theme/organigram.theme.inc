<?php
/**
 * @file
 * Defines the theme functions used by the organigram module.
 */

/**
 * Theme function for the form 'organigram_form_overview_organigrams'.
 *
 * @param $variables
 *   An array containing form elements.
 *
 * @return string
 *   Rendered form.
 *
 * @throws \Exception
 */
function theme_organigram_form_overview_organigrams($variables) {
  // Retrieve the form element.
  $form = $variables['form'];
  // Initialize rows array.
  $rows = array();
  // Iterate through form child elements.
  foreach (element_children($form['items']) as $key) {
    // Retrieve form element by reference.
    $child_element = &$form['items'][$key];
    // Only render row if the name element is set.
    if (isset($child_element['name'])) {
      // Initialize row variable.
      $row = array();
      $row[] = drupal_render($child_element['name']);
      // The weight column is not included if there is only one organigram.
      if (isset($child_element['weight'])) {
        // Add the 'organigram-weight' to the weight element class attribute.
        $child_element['weight']['#attributes']['class'] = array('organigram-weight');
        // Render weight element.
        $row[] = drupal_render($child_element['weight']);
      }
      // Render operation links.
      $row[] = drupal_render($child_element['list']);
      $row[] = drupal_render($child_element['add']);
      $row[] = drupal_render($child_element['view']);
      $row[] = drupal_render($child_element['edit']);
      $row[] = drupal_render($child_element['delete']);
      // Add row to the rows array.
      $rows[] = array('data' => $row, 'class' => array('draggable'));
    }
  }
  // Construct table header.
  $header = array(t('Organigram name'));
  // The weight column is omitted if less then two rows are present.
  if (isset($form['actions'])) {
    $header[] = t('Weight');
    drupal_add_tabledrag('organigram', 'order', 'sibling', 'organigram-weight');
  }
  // Add operation header.
  $header[] = array('data' => t('Operations'), 'colspan' => '5');
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => $form['#empty_text'],
    'attributes' => array(
      'id' => 'organigram',
    ),
  )) . drupal_render_children($form);
}

/**
 * Theme function for the form 'organigram_form_overview_organigram_items'.
 *
 * @param array $variables
 *   An array containing form elements.
 *
 * @return string
 *   Rendered form.
 *
 * @throws \Exception
 */
function theme_organigram_form_overview_organigram_items($variables) {
  $form = $variables['form'];
  // Add support for table drag.
  drupal_add_tabledrag('organigram_items', 'order', 'sibling', 'organigram-item-weight');
  drupal_add_tabledrag('organigram_items', 'match', 'parent', 'organigram-item-parent', 'organigram-item-parent', 'organigram-item-id', FALSE);
  drupal_add_tabledrag('organigram_items', 'depth', 'group', 'organigram-item-depth', NULL, NULL, FALSE);
  // Initialize rows array.
  $rows = array();
  // Iterate through the element child names.
  foreach (element_children($form['items']) as $key) {
    // Retrieve the organigram item element by reference.
    $child_element = &$form['items'][$key];
    // Retrieve the organigram item.
    $organigram_item = $child_element['#organigram_item'];
    // Initialize row array.
    $row = array();
    $row[] = theme('indentation', array('size' => $organigram_item->depth)) . drupal_render($child_element['view']);
    // Add the correct classes which are needed for the tabledrag to function.
    $child_element['iid']['#attributes']['class'] = array('organigram-item-id');
    $child_element['parent']['#attributes']['class'] = array('organigram-item-parent');
    $child_element['depth']['#attributes']['class'] = array('organigram-item-depth');
    $child_element['weight']['#attributes']['class'] = array('organigram-item-weight');
    // Add the iid, parent and depth to the first cell.
    $row[0] .= drupal_render($child_element['iid']) . drupal_render($child_element['parent']) . drupal_render($child_element['depth']);
    $row[] = drupal_render($child_element['weight']);
    $row[] = drupal_render($child_element['edit']);
    $row[] = drupal_render($child_element['delete']);
    // Add the row to the rows array.
    $rows[] = array('data' => $row, 'class' => array('draggable'));
  }

  $header = array(
    t('Name'),
    t('Weight'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => $form['#empty_text'],
    'attributes' => array(
      'id' => 'organigram_items',
    ),
  )) . drupal_render_children($form);
}
