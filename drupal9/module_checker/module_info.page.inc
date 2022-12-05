<?php

/**
 * @file
 * Contains module_info.page.inc.
 *
 * Page callback for Module info entities.
 */

use Drupal\Core\Render\Element;

/**
 * Prepares variables for Module info templates.
 *
 * Default template: module_info.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_module_info(array &$variables) {
  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
