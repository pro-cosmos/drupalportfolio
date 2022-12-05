<?php

namespace Drupal\module_checker\Entity;

use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityPublishedTrait;
use Drupal\Core\Entity\EntityTypeInterface;

/**
 * Defines the Module info entity.
 *
 * @ingroup module_checker
 *
 * @ContentEntityType(
 *   id = "module_info",
 *   label = @Translation("Module info"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\module_checker\ModuleInfoListBuilder",
 *     "views_data" = "Drupal\module_checker\Entity\ModuleInfoViewsData",
 *     "translation" = "Drupal\module_checker\ModuleInfoTranslationHandler",
 *
 *     "form" = {
 *       "default" = "Drupal\module_checker\Form\ModuleInfoForm",
 *       "add" = "Drupal\module_checker\Form\ModuleInfoForm",
 *       "edit" = "Drupal\module_checker\Form\ModuleInfoForm",
 *       "delete" = "Drupal\module_checker\Form\ModuleInfoDeleteForm",
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\module_checker\ModuleInfoHtmlRouteProvider",
 *     },
 *     "access" = "Drupal\module_checker\ModuleInfoAccessControlHandler",
 *   },
 *   base_table = "module_info",
 *   data_table = "module_info_field_data",
 *   translatable = TRUE,
 *   admin_permission = "administer module info entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "name",
 *     "uuid" = "uuid",
 *     "langcode" = "langcode",
 *     "published" = "status",
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/module_info/{module_info}",
 *     "add-form" = "/admin/structure/module_info/add",
 *     "edit-form" = "/admin/structure/module_info/{module_info}/edit",
 *     "delete-form" = "/admin/structure/module_info/{module_info}/delete",
 *     "collection" = "/admin/structure/module_info",
 *   },
 *   field_ui_base_route = "module_info.settings"
 * )
 */
class ModuleInfo extends ContentEntityBase implements ModuleInfoInterface {

  use EntityChangedTrait;
  use EntityPublishedTrait;

  /**
   * {@inheritdoc}
   */
  public function getName() {
    return $this->get('name')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    // Add the published field.
    $fields += static::publishedBaseFieldDefinitions($entity_type);

    $fields['name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Name'))
      ->setDescription(t('The name of the Module info entity.'))
      ->setSettings([
        'max_length' => 50,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setRequired(TRUE)
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -4,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['machine_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Machine name'))
      ->setDescription(t('The module machine name.'))
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -4,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['version'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Version'))
      ->setDescription(t('The module version.'))
      ->setSettings([
        'max_length' => 50,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
        'weight' => -4,
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -4,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['sites'] = BaseFieldDefinition::create('integer')
      ->setLabel(t('Sites'))
      ->setDescription(t('Module number of used sites.'))
      ->setDefaultValue(0)
      ->setDisplayOptions('form', [
        'type' => 'number',
        'weight' => -4,
      ]);
    $fields['issues'] = BaseFieldDefinition::create('integer')
      ->setLabel(t('Issues'))
      ->setDescription(t('Open Issues of the module.'))
      ->setDefaultValue(0)
      ->setDisplayOptions('form', [
        'type' => 'number',
        'weight' => -4,
      ]);
    $fields['bugs'] = BaseFieldDefinition::create('integer')
      ->setLabel(t('Bugs'))
      ->setDescription(t('Open bugs of the module.'))
      ->setDefaultValue(0)
      ->setDisplayOptions('form', [
        'type' => 'number',
        'weight' => -4,
      ]);

    $fields['status']->setDescription(t('A boolean indicating whether the Module info is published.'))
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
        'weight' => -3,
      ]);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the entity was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the entity was last edited.'));

    return $fields;
  }

}
