<?php

namespace Drupal\bgm_mini_client_test\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Class implements BGM Mini service endpoint.
 */
class TestController extends ControllerBase {

  /**
   * Implement remote login test.
   *
   * @return string string
   *
   */
  public function remote_login_test(string $token) {
    /** @var \Drupal\bgm_mini_client\BgmMiniClientService $BgmMiniClient */
    $BgmMiniClient = \Drupal::service('bgm_mini_client.client_service');
    $decoded = (array) $BgmMiniClient->decode($token);
    $uuid = $decoded['uuid'] ?? '';
    // Return decoded values for assertion checking.
    $message = 'JWT decoded successfully uid:' . $uuid;
    return ['#markup' => $message];
  }

}
