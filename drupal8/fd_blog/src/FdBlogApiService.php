<?php

namespace Drupal\fd_blog;

use Drupal\Component\Serialization\Json;
use Drupal\Core\Config\ConfigFactoryInterface;
use GuzzleHttp\ClientInterface;
use GuzzleHttp\Exception\RequestException;

/**
 * Class FdBlogApiService.
 */
class FdBlogApiService {

  /**
   * The update settings
   *
   * @var \Drupal\Core\Config\Config
   */
  protected $settings;

  /**
   * The HTTP client to fetch the feed data with.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $client;

  /**
   * Constructs a UpdateFetcher.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   * @param \GuzzleHttp\ClientInterface $http_client
   *   A Guzzle client object.
   */
  public function __construct(ConfigFactoryInterface $config_factory, ClientInterface $http_client) {
    $this->client = $http_client;
    $this->settings = $config_factory->get('fd_blog.fdconfig');
  }

  /**
   * Return all Posts.
   * @return array|mixed
   */
  public function getPostsAll() {
    $data = $this->getData($this->settings->get('api_path_posts_all'));
    if ($data) {
      $limit = $this->settings->get('posts_number');
      $data = $limit ? array_slice($data, 0, $limit) : $data;
      return $data;
    }
    return [];
  }

  /**
   * Return data for single Post.
   *
   * @param $id
   * @return mixed
   */
  public function getPost($id){
    $url = str_replace('[post_id]', $id, $this->settings->get('api_path_posts_single'));
    return $this->getData($url);
  }

  /**
   * Request to endpoint.
   *
   * @param $path
   * @return mixed
   */
  private function getData($path) {
   $request_url = $this->settings->get('api_url') . '/' .$path;
    try {
      $data = (string)$this->client
        ->get($request_url, ['headers' => [
          'Accept' => 'application/json',
          'Content-Type' => 'application/x-www-form-urlencoded',]
        ])
        ->getBody();
      return Json::decode($data);
    }
    catch (RequestException $exception) {
      watchdog_exception('fd_blog', $exception);
    }
  }


}
