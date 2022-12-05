<?php

namespace Drupal\module_checker\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a 'ModulesInfo' block.
 *
 * @Block(
 *  id = "modules_info",
 *  admin_label = @Translation("Modules info"),
 * )
 */
class ModulesInfo extends BlockBase implements ContainerFactoryPluginInterface {

  /**
   * Drupal\module_checker\ModuleChecker definition.
   *
   * @var \Drupal\module_checker\ModuleChecker
   */
  protected $moduleCheckerService;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    $instance = new static($configuration, $plugin_id, $plugin_definition);
    $instance->moduleCheckerService = $container->get('module_checker.service');
    return $instance;
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'modules_display_count' => 10,
    ] + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form['modules_display_count'] = [
      '#type' => 'number',
      '#title' => $this->t('Modules display count'),
      '#default_value' => $this->configuration['modules_display_count'],
      '#weight' => '0',
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function blockSubmit($form, FormStateInterface $form_state) {
    $this->configuration['modules_display_count'] = $form_state->getValue('modules_display_count');
  }

  /**
   * {@inheritdoc}
   */
  public function build() {
    $build = [];
    $build['#theme'] = 'modules_info';
    $count = $this->configuration['modules_display_count'];
    $rows = \Drupal::service('module_checker.service')->getModulesInfoData($count);
    $values = [];
    foreach ($rows as $row) {
      $values[] = [
        'name' => $row->label(),
        'version' => $row->get('version')->value,
        'sites' => $row->get('sites')->value,
        'issues' => $row->get('issues')->value,
        'bugs' => $row->get('bugs')->value,
      ];
    }

    $build['#content'] = $values;
    return $build;
  }

}
