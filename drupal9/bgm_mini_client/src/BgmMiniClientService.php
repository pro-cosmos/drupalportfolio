<?php

declare(strict_types=1);

namespace Drupal\bgm_mini_client;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\user\Entity\User;
use Firebase\JWT\JWT;
use GuzzleHttp\ClientInterface;
use function implode;

/**
 * Class BgmMiniClientService.
 */
class BgmMiniClientService {
  /**
   * GuzzleHttp\ClientInterface definition.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $httpClient;

  /**
   * Drupal\Core\Config\ConfigFactoryInterface definition.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $config;

  /**
   * The JWT Transcoder service.
   *
   * @var \Drupal\jwt\Transcoder\JwtTranscoderInterface
   */
  protected $transcoder;

  /**
   * Constructs a new VivamindApiService object.
   */
  public function __construct(ClientInterface $http_client, ConfigFactoryInterface $config_factory, JWT $php_jwt) {
    $this->httpClient = $http_client;
    $this->transcoder = $php_jwt;
    $this->config = $config_factory->getEditable('bgm_mini_client.settings');
  }

  /**
   * Return login link for given user.
   *
   * @param int $uid
   *   Uid of user.
   *
   * @return string
   */
  public function getUserLoginUrl($uid = NULL) {
    // Generate JWT token.
    $token = $this->getUserToken($uid);
    // Create url for redirection.
    $url = $this->getLoginLink($token);
    return $url;
  }

  /**
   * Return login link for given user.
   *
   * @param int $uid
   *   Uid of user.
   *
   * @return string
   */
  public function getUserToken($uid = NULL) {
    // If uid is empty get current user.
    if(!$uid) {
      $user = User::load(\Drupal::currentUser()->id());
    }
    $uuid = $user->get('field_external_id')->value;
    $data = ['uuid' => $uuid];
    // Generate JWT token.
    return $this->buildToken($data);
  }

  /**
   * Return the url of remote login endpoint.
   *
   * @param string $token
   *   JWT token.
   *
   * @return string
   */
  public function getLoginLink(string $token) {
    $base_url = $this->config->get('remote_login_uri') ?? '';
    return implode('/', [$base_url, $token]);
  }

  /**
   * Generate JWT token with given custom data.
   *
   * @param array $data
   *   Custom data needs to add in payload.
   *
   * @return string
   */
  public function buildToken(array $data): string {
    $payload = [
      'alg' => 'HS256',
      'iss' => $_SERVER['SERVER_ADDR'],
      'aud' => $this->config->get('remote_login_uri'),
      'iat' => \Drupal::time()->getCurrentTime(),
      'exp' => \Drupal::time()->getCurrentTime() + 15,
    ];
    if ($data) {
      $payload += $data;
    }
    // Get secret private key.
    $key = $this->getKey();

    return $this->transcoder::encode($payload, $key, 'HS256');
  }

  /**
   * Returns the secret key required to create the JWT token.
   *
   * @return string
   */
  public function getKey(): string
  {
    $key = getenv('BGM_MINI_JWT_KEY') ?? '';
    if (!$key) {
      \Drupal::logger('bgm_mini_client')->error('BGM Mini jwt key is undefined!');
    }
    return $key;
  }

  public function decode(string $token){
    try {
      return (array) $this->transcoder::decode($token, $this->getKey(), ['HS256']);
    } catch (\Exception $e) {
      \Drupal::logger('bgm_mini_service')->error($e->getMessage());
    }
  }

}
