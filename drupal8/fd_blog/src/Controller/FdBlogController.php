<?php

namespace Drupal\fd_blog\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class FdBlogController.
 */
class FdBlogController extends ControllerBase {

  /**
   * Drupal\fd_blog\FdBlogApiService definition.
   *
   * @var \Drupal\fd_blog\FdBlogApiService
   */
  protected $fdBlogApi;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    $instance = parent::create($container);
    $instance->fdBlogApi = $container->get('fd_blog.api');
    return $instance;
  }

  /**
   * @param string $id
   * @return array
   */
  public function show($id) {
    // Show single post.
    if ((int)$id > 0) {
      $data = \Drupal::service('fd_blog.api')->getPost((int)$id);
      return [
        '#theme' => 'fd_blog_post',
        '#item' => $data,
      ];
    }

    // By default shoe all posts.
    $data = \Drupal::service('fd_blog.api')->getPostsAll();
    return [
      '#theme' => 'fd_blog',
      '#items' => $data,
    ];
  }

}
