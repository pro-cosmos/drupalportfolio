<?php

namespace Drupal\bgm_mini_client\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Routing\TrustedRedirectResponse;

/**
 * Class BgmMiniLoginController.
 */
class BgmMiniLoginController extends ControllerBase {

  /**
   * Drupal\Core\Config\ConfigManagerInterface definition.
   *
   * @var \Drupal\Core\Config\ConfigManagerInterface
   */
  protected $configManager;

  /**
   * GuzzleHttp\ClientInterface definition.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $httpClient;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    $instance = parent::create($container);
    $instance->configManager = $container->get('config.manager');
    $instance->httpClient = $container->get('http_client');
    return $instance;
  }

  /**
   * Content.
   *
   * @return string
   *
   */
  public function callback()
  {
    /** @var \Drupal\bgm_mini_client\BgmMiniClientService $BgmMiniClient */
    $BgmMiniClient = \Drupal::service('bgm_mini_client.client_service');
    $redirect_url = $BgmMiniClient->getUserLoginUrl();
    // Redirect to external url.
    return new TrustedRedirectResponse($redirect_url);
  }
}
