<?php

namespace Drupal\pog_multistep\Plugin\Derivative;

use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\pog_multistep\Utility\EntityFormModeHelper;
use Symfony\Component\DependencyInjection\ContainerInterface;

class MultistepMenuDeriver extends DeriverBase implements ContainerDeriverInterface {

  /**
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  public function __construct($base_plugin_id, EntityTypeManagerInterface $entityTypeManager) {
    $this->entityTypeManager = $entityTypeManager;
  }

  /**
   * Creates a new class instance.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The container to pull out services used in the fetcher.
   * @param string $base_plugin_id
   *   The base plugin ID for the plugin ID.
   *
   * @return static
   *   Returns an instance of this fetcher.
   */
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static(
      $base_plugin_id,
      $container->get('entity_type.manager')
    );
  }

  /**
   * Gets the definition of all derivatives of a base plugin.
   *
   * @param array $base_plugin_definition
   *   The definition array of the base plugin.
   *
   * @return array
   *   An array of full derivative definitions keyed on derivative id.
   *
   * @see getDerivativeDefinition()
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    $links = [];
    $tab_modes = EntityFormModeHelper::getTabModes();

    /** @var \Drupal\Core\Entity\Entity\EntityFormMode $mode */
    foreach ($tab_modes as $tab_index => $mode) {
      $op = EntityFormModeHelper::getOperationName($mode);
      $title = $mode->label();

      $links[$op] = [
          'title' => $title,
          'description' => $mode->label(),
          'route_name' => 'pog_multistep.multistep_node_controller_add',
          'route_parameters' => [
            'entity_form_mode' => $op,
          ],
          'weight' => $tab_index,
        ] + $base_plugin_definition;
    }

    // Add last menu item to view result POG node.
    if (!isset($links['pog'])) {
      $links['pog'] = [
          'title' => t('POG'),
          'description' => t('POG'),
          'route_name' => 'pog_multistep.multistep_node_controller_redirect',
          'weight' => 99,
        ] + $base_plugin_definition;
    }

    return $links;
  }
}
