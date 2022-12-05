<?php

namespace Drupal\bgm_mini_client;

use Symfony\Component\Routing\Route;

/**
 * Provides dynamic routes for local login redirection.
 */
class Routes
{

  public function dynamic()
  {
    $config = \Drupal::service('config.factory');
    $local_login_uri = $config->get('bgm_mini_client.settings')->get('local_login_uri') ?? '/test/redirect';
    $route = new Route(
    // Path to attach this route to:
      $local_login_uri,
      // Route defaults:
      [
        '_controller' => '\Drupal\bgm_mini_client\Controller\BgmMiniLoginController::callback',
      ],
      // Route requirements:
      [
        '_role' => 'authenticated'
      ]
    );
    return ['bgm_mini_login' => $route];
  }

}
