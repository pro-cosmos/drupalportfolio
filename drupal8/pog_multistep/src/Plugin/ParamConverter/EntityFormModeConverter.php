<?php

namespace Drupal\pog_multistep\Plugin\ParamConverter;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityRepositoryInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\ParamConverter\ParamConverterInterface;
use Drupal\Core\TypedData\TranslatableInterface;
use Symfony\Component\Routing\Route;

/**
 * Class EntityFormModeConverter.
 */
class EntityFormModeConverter implements ParamConverterInterface {

  const PREFIX = 'entity_form_mode:';

  /**
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  public function __construct(EntityTypeManagerInterface $entityTypeManager) {
    $this->entityTypeManager = $entityTypeManager;
  }

  /**
   * Converts path variables to their corresponding objects.
   *
   * @param mixed $value
   *   The raw value.
   * @param mixed $definition
   *   The parameter definition provided in the route options.
   * @param string $name
   *   The name of the parameter.
   * @param array $defaults
   *   The route defaults array.
   *
   * @return mixed|null
   *   The converted parameter value.
   */
  public function convert($value, $definition, $name, array $defaults) {
    if (!empty($definition['type'])) {
      $entity_type_id = $this->getEntityTypeId($definition['type']);
      $entity = $this->entityTypeManager->getStorage('entity_form_mode')
        ->load("$entity_type_id.$value");

      return $entity;
    }

    return NULL;
  }

  /**
   * Determines if the converter applies to a specific route and variable.
   *
   * @param mixed $definition
   *   The parameter definition provided in the route options.
   * @param string $name
   *   The name of the parameter.
   * @param \Symfony\Component\Routing\Route $route
   *   The route to consider attaching to.
   *
   * @return bool
   *   TRUE if the converter applies to the passed route and parameter, FALSE
   *   otherwise.
   */
  public function applies($definition, $name, Route $route) {
    if (!empty($definition['type']) && strpos($definition['type'], static::PREFIX) === 0) {
      $entity_type_id = $this->getEntityTypeId($definition['type']);
      // TODO: Implement dynamic routing - see \Drupal\Core\ParamConverter\EntityConverter::applies()
      return $this->entityTypeManager->hasDefinition($entity_type_id);
    }

    return FALSE;
  }

  protected function getEntityTypeId($type) {
    return substr($type, strlen(static::PREFIX));
  }
}
