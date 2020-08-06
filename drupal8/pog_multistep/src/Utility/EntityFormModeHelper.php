<?php

namespace Drupal\pog_multistep\Utility;

use Drupal\Core\Entity\EntityFormModeInterface;

abstract class EntityFormModeHelper {
  /**
   * @param \Drupal\Core\Entity\EntityFormModeInterface $entityFormMode
   *
   * @return string
   *    Form operation name of entity form mode.
   */
  public static function getOperationName(EntityFormModeInterface $entityFormMode) {
    $target_type = $entityFormMode->getTargetType();

    // Entity form mode ID begins with target entity type ID followed by a dot.
    return substr($entityFormMode->id(), strlen($target_type) + 1);
  }

  /**
   * @return array
   *   All tab-enabled entity form modes.
   */
  public static function getTabModes() {
    // TODO: Refactor this class into a service with DI.
    $modes = \Drupal::entityTypeManager()
      ->getStorage('entity_form_mode')
      ->loadMultiple();
    $tab_modes = [];

    /** @var \Drupal\Core\Entity\Entity\EntityFormMode $mode */
    foreach ($modes as $mode_id => $mode) {
      $tab_index = (int) $mode->getThirdPartySetting('pog_multistep', 'tab_index', 0);

      if ($tab_index > 0) {
        $tab_modes[$tab_index] = $mode;
      }
    }

    return $tab_modes;
  }
}
