<?php

namespace Drupal\fd_blog\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class FdConfigForm.
 */
class FdConfigForm extends ConfigFormBase {

  /**
   * Symfony\Component\HttpKernel\HttpKernelInterface definition.
   *
   * @var \Symfony\Component\HttpKernel\HttpKernelInterface
   */
  protected $httpKernelBasic;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    $instance = parent::create($container);
    $instance->httpKernelBasic = $container->get('http_kernel.basic');
    return $instance;
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'fd_blog.fdconfig',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'fd_config_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('fd_blog.fdconfig');
    $form['api_url'] = [
      '#type' => 'url',
      '#title' => $this->t('API-url'),
      '#description' => $this->t('Provide source API-url'),
      '#default_value' => $config->get('api_url'),
    ];
    $form['api_path_posts_all'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Overview posts endpoint path'),
      '#description' => $this->t('Provide url for all posts'),
      '#default_value' => $config->get('api_path_posts_all'),
    ];
    $form['api_path_posts_single'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Single post endpoint path'),
      '#description' => $this->t('Provide url for single post'),
      '#default_value' => $config->get('api_path_posts_single'),
    ];
    $form['posts_number'] = [
      '#type' => 'number',
      '#title' => $this->t('Number of posts'),
      '#description' => $this->t('Number posts on Blog posts page'),
      '#default_value' => $config->get('posts_number'),
    ];
    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    parent::submitForm($form, $form_state);

    $this->config('fd_blog.fdconfig')
      ->set('api_url', $form_state->getValue('api_url'))
      ->set('api_path_posts_all', $form_state->getValue('api_path_posts_all'))
      ->set('api_path_posts_single', $form_state->getValue('api_path_posts_single'))
      ->set('posts_number', $form_state->getValue('posts_number'))
      ->save();
  }

}
