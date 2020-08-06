<?php

namespace Drupal\pog_multistep\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityDisplayRepositoryInterface;
use Drupal\Core\Entity\EntityFormModeInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\pog_multistep\Utility\EntityFormModeHelper;
use Drupal\Core\Url;
use Drupal\Core\Ajax;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;
use Drupal\Core\Ajax\CloseModalDialogCommand;
use Drupal\Core\Ajax\RedirectCommand;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;

/**
 * Class MultistepNodeController.
 */
class MultistepNodeController extends ControllerBase {

  /**
   * A repository for entity display objects.
   *
   * @var \Drupal\Core\Entity\EntityDisplayRepositoryInterface
   */
  protected $entityDisplayRepository;

  /**
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected $currentUser;

  public function __construct(EntityDisplayRepositoryInterface $entityDisplayRepository, AccountInterface $currentUser) {
    $this->entityDisplayRepository = $entityDisplayRepository;
    $this->currentUser = $currentUser;
  }

  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('entity_display.repository'),
      $container->get('current_user')
    );
  }

  /**
   * Provides the node submission form.
   *
   * @param EntityFormModeInterface $entity_form_mode
   *
   * @return array
   *   A node submission form.
   */
  public function add(EntityFormModeInterface $entity_form_mode) {
    $node = pog_multistep_get_current_node();
    if (!$node) {
      $node = \Drupal::entityTypeManager()->getStorage('node')->create([
        'type' => 'pog',
      ]);
    }

    $op = EntityFormModeHelper::getOperationName($entity_form_mode);
    $form = $this->entityFormBuilder()->getForm($node, $op);
    return $form;
  }

  /**
   * Provides the node submission form.
   *
   * @param EntityFormModeInterface $entity_form_mode
   *
   * @return array
   *   A node submission form.
   */
  public function redir() {
    if ($node = pog_multistep_get_current_node()){
      $path = UrL::fromUserInput('/node/' . $node->id())->toString();
    }
    return new RedirectResponse($path);
  }

  /**
   * Provides the POG accept form.
   *
   * @return array
   *   A node submission form.
   */
  public function accept() {
    if ($node = pog_multistep_get_current_node()) {
      $form = \Drupal::formBuilder()->getForm('Drupal\pog_multistep\Form\MultistepAcceptForm', $node);
      $response = new AjaxResponse();
      $response->addCommand(new OpenModalDialogCommand(' ', $form));
      return $response;
    }
  }
}
