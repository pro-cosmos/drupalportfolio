<?php

namespace Drupal\module_checker;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Module info entity.
 *
 * @see \Drupal\module_checker\Entity\ModuleInfo.
 */
class ModuleInfoAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\module_checker\Entity\ModuleInfoInterface $entity */

    switch ($operation) {

      case 'view':

        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished module info entities');
        }

        return AccessResult::allowedIfHasPermission($account, 'view published module info entities');

      case 'update':

        return AccessResult::allowedIfHasPermission($account, 'edit module info entities');

      case 'delete':

        return AccessResult::allowedIfHasPermission($account, 'delete module info entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add module info entities');
  }

}
