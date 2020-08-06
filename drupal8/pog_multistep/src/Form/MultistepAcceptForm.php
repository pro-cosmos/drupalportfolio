<?php

namespace Drupal\pog_multistep\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Cache\Cache;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\RedirectCommand;

/**
 * Class MultistepAcceptForm.
 */
class MultistepAcceptForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pog_multistep_accept_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $node = $form_state->getBuildInfo()['args'][0];
    $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $form['nid'] = [
      '#type' => 'hidden',
      '#default_value' => $node->id(),
    ];

    $form['text'] = [
      '#markup' => $this->t('<ul>
	<li>Die Inhalte auf dieser Webseite und insbesondere der POG-Generator wurden mit größtmöglicher Sorgfalt erstellt.</li>
	<li>Der Betreiber dieser Webseite übernimmt jedoch keine Gewähr für die Richtigkeit und Aktualität der Inhalte auf dieser Website und/oder die Funktionalitäten des POG-Generators.</li>
	<li>Insbesondere übernimmt der Betreiber dieser Webseite&nbsp;&nbsp;keine Gewähr für die Richtigkeit und/oder Vollständigkeit eines mit Hilfe des POG-Generator erstellten POG-Dokuments.</li>
	<li>Der hier zur Verfügung gestellte POG-Generator ist nicht dafür konzipiert und nicht in der Lage, ein jeweils konkret auf den Einzelfall angepasstes, umfassendes, vollständiges und sämtlichen rechtlichen Vorschriften Rechnung tragendes POG-Dokument zu erstellen.</li>
	<li>Der POG-Generator liefert lediglich auf Grundlage der vom Nutzer eigenverantwortlich eingegebenen Daten eine Grundlage für das POG-Dokument.</li>
	<li>Dem Nutzer wird daher dringend empfohlen, ein mit Hilfe des hier zur Verfügung gestellten POG-Generators erstelltes POG-Grundlagen-Dokument selbst zu prüfen und zu vervollständigen oder prüfen zu lassen.&nbsp;</li>
</ul>'),
    ];

    $form['accept_checkbox'] = [
      '#type' => 'checkbox',
      '#required' => TRUE,
      '#title' => t('Ich bestätige den Disclaimer gelesen und verstanden zu haben. Dies gilt insbesondere für den Punkt 1 Allgemeines'),
    ];

    $form['actions']['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Confirm'),
      '#states' => [
        'disabled' => ['[name="accept_checkbox"]' => ['checked' => FALSE]]
      ],
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
        $entity->set('field_pog_accepted', TRUE);
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
