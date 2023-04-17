<?php

/**
 * @file
 * File contain the product total calculation form.
 */

/**
 * Return the tax map information.
 *
 * @return array[]
 */
function _mchs_test_get_tax_map() {
  return [
    'MR' => ['Марс', 19],
    'ST' => ['Сатурн', 22],
    'JP' => ['Юпитер', 24],
  ];
}

/**
 * Return the products information.
 *
 * @return \string[][]
 */
function _mchs_test_get_products() {
  return [
    ['Наушники', '100', 'евро'],
    ['Чехол для телефона', '20', 'евро'],
    ['Телефон', '500', 'евро'],
  ];
}

/**
 * Return the tax planet information by given tax number.
 *
 * @param string $tax
 *   Tax number.
 *
 * @return false|mixed
 */
function _mchs_test_get_tax_info($tax) {
  // Validate tax number format.
  if (!preg_match("/([A-z]+)(\d{9})$/", $tax, $matches)) {
    return FALSE;
  }

  $tax_key = $matches[1];
  $tax_map = _mchs_test_get_tax_map();
  return isset($tax_map[$tax_key])? $tax_map[$tax_key] : FALSE;
}

/**
 * Product total calculation form.
 *
 */
function product_total_calc_form($form, &$form_state) {
  $products_op = [];
  foreach (_mchs_test_get_products() as $key => $product) {
    $products_op[$key] = $product[0] . ' (' . $product[1] . ' ' . $product[2] . ')';
  };

  $tax_map = _mchs_test_get_tax_map();
  $form['product'] = [
    '#type' => 'select',
    '#title' => t('Products'),
    '#default_value' => 0,
    '#options' => $products_op,
    '#required' => TRUE,
  ];
  $form['tax'] = [
    '#type' => 'textfield',
    '#title' => t('Tax number'),
    '#required' => TRUE,
    '#default_value' => '',
    '#description' => t('Example of valid tax keys: @list', ['@list' => implode(', ', array_keys($tax_map))]),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Submit',
  ];
  return $form;
}

/**
 * Validate handler for product_total_calc_form().
 */
function product_total_calc_form_validate($form, &$form_state) {
  $tax = trim($form_state['values']['tax']);
  $tax_info = _mchs_test_get_tax_info($tax);
  if (!$tax_info) {
    form_set_error('tax', t('Enter a valid tax number.'));
  }
}

/**
 * Submit handler for product_total_calc_form().
 */
function product_total_calc_form_submit($form, &$form_state) {
  // Get product info.
  $products = _mchs_test_get_products();
  $product_key = $form_state['values']['product'];
  $product = $products[$product_key];

  // Get user tax percent.
  $tax = trim($form_state['values']['tax']);
  $tax_info = _mchs_test_get_tax_info($tax);
  $percent = $tax_info[1];
  $planet = $tax_info[0];
  $currency = $product[2];

  // Calculate product total price.
  $total_price = $product[1] + $product[1] * $percent / 100;

  drupal_set_message(t('Total price of "@product" = @total @currency (+@percent% is @planet tax)',
    [
      '@product' => $product[0],
      '@percent' => $percent,
      '@total' => $total_price,
      '@planet' => $planet,
      '@currency' => $currency,
    ]
  ));

  // Rebuild the form and keep the filter values.
  $form_state['rebuild'] = TRUE;
}