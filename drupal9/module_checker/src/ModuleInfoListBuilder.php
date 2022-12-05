<?php

namespace Drupal\module_checker;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;

/**
 * Defines a class to build a listing of Module info entities.
 *
 * @ingroup module_checker
 */
class ModuleInfoListBuilder extends EntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['id'] = $this->t('Module info ID');
    $header['name'] = $this->t('Name');
    $header['version'] = $this->t('Version');
    $header['sites'] = $this->t('Sites');
    $header['bugs'] = $this->t('Bugs');
    $header['issues'] = $this->t('Issues');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /** @var \Drupal\module_checker\Entity\ModuleInfo $entity */
    $row['id'] = $entity->id();
    $row['name'] = Link::createFromRoute(
      $entity->label(),
      'entity.module_info.edit_form',
      ['module_info' => $entity->id()]
    );
    $row['version'] = $entity->get('version')->value;
    $row['sites'] = $entity->get('sites')->value;
    $row['bugs'] = $entity->get('bugs')->value;
    $row['issues'] = $entity->get('issues')->value;
    return $row + parent::buildRow($entity);
  }

}
