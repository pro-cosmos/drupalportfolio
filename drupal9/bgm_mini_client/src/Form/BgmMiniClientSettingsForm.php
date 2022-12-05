<?php

namespace Drupal\bgm_mini_client\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class BgmMiniClientSettingsForm.
 */
class BgmMiniClientSettingsForm extends ConfigFormBase {

  /**
   * Drupal\Core\Config\ConfigManagerInterface definition.
   *
   * @var \Drupal\Core\Config\ConfigManagerInterface
   */
  protected $configManager;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    $instance = parent::create($container);
    $instance->configManager = $container->get('config.manager');
    return $instance;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'bgm_mini_client_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('bgm_mini_client.settings');
    $form['local_login_uri'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Local url for BGM Mini login redirection page'),
      '#default_value' => $config->get('local_login_uri'),
    ];
    if (!empty($config->get('local_login_uri'))) {
      $url =  $config->get('local_login_uri');
      $url = Url::fromUserInput($url, ['absolute' => true, 'attributes' =>['target'=>'_blank']]);
      $link = Link::fromTextAndUrl($url->toString(), $url);

      $form['local_login_uri']['#description'] = $this->t('Use this link @url for login via BGM Mini service',
        ['@url' => $link->toString()]
      );
    }

    $form['remote_login_uri'] = [
      '#type' => 'url',
      '#title' => $this->t('Remote url for login on BGM Mini'),
      '#default_value' => $config->get('remote_login_uri'),
      '#attributes' => [
        'placeholder' => 'http://',
      ],
      '#required' => TRUE
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    parent::submitForm($form, $form_state);
    $local_uri = '/' . ltrim(trim($form_state->getValue('local_login_uri')), '/');
    $this->config('bgm_mini_client.settings')
      ->set('local_login_uri', $local_uri)
      ->set('remote_login_uri', $form_state->getValue('remote_login_uri'))
      ->save();

    // Clear cache for new dynamic url.
    \Drupal::service('router.builder')->setRebuildNeeded();
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'bgm_mini_client.settings',
    ];
  }

}
