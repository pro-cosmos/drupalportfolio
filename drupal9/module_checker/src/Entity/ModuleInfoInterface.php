<?php

namespace Drupal\module_checker\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\Core\Entity\EntityPublishedInterface;

/**
 * Provides an interface for defining Module info entities.
 *
 * @ingroup module_checker
 */
interface ModuleInfoInterface extends ContentEntityInterface, EntityChangedInterface, EntityPublishedInterface {

  /**
   * Add get/set methods for your configuration properties here.
   */

  /**
   * Gets the Module info name.
   *
   * @return string
   *   Name of the Module info.
   */
  public function getName();

  /**
   * Sets the Module info name.
   *
   * @param string $name
   *   The Module info name.
   *
   * @return \Drupal\module_checker\Entity\ModuleInfoInterface
   *   The called Module info entity.
   */
  public function setName($name);

  /**
   * Gets the Module info creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Module info.
   */
  public function getCreatedTime();

  /**
   * Sets the Module info creation timestamp.
   *
   * @param int $timestamp
   *   The Module info creation timestamp.
   *
   * @return \Drupal\module_checker\Entity\ModuleInfoInterface
   *   The called Module info entity.
   */
  public function setCreatedTime($timestamp);

}
