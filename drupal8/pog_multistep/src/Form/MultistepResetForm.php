<?php

namespace Drupal\pog_multistep\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\RedirectCommand;

/**
 * Class MultistepResetForm.
 */
class MultistepResetForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pog_multistep_reset_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $nid = $form_state->getBuildInfo()['args'][0];
    $nid = $nid->getValue()[0]['value'];
    $node = \Drupal\node\Entity\Node::load($nid);
    $show_reset_button = strtotime($node->get('field_date_html_generation')->value) < $node->get('changed')->value;
    $class = $show_reset_button? '': 'hidden';

    $form['nid'] = [
     '#type' => 'hidden',
     '#default_value' => $nid,
    ];

    $form['button'] = [
      '#type' => 'submit',
      '#value' => $this->t('Änderungen verwerfen'),
      '#id' => 'pog_multistep_button',
      '#attributes' => ['class' => [$class]],
      '#ajax' => [
        'callback' => '::ajaxSubmitCallback',
        'event' => 'click',
        'progress' => [
          'type' => 'throbber',
        ]
      ]
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    if ($nid = $form_state->getValue('nid')) {
      if ($entity = \Drupal\node\Entity\Node::load($nid)) {
        $entity->set('field_date_html_generation', '');
        $entity->isComplete = TRUE;
        $entity->save();
      }
    }
  }

  public function ajaxSubmitCallback(array &$form, FormStateInterface $form_state) {
    $response = new AjaxResponse();
    $nid = $form_state->getValue('nid');
    $response->addCommand(new RedirectCommand('/node/' . $nid));
    return $response;
  }

}
