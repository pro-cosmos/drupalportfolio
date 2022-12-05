<?php

namespace Drupal\module_checker;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use GuzzleHttp\ClientInterface;
use GuzzleHttp\Exception\TransferException;

/**
 * Class ModuleChecker implements common tasks with module info entities.
 */
class ModuleChecker {

  /**
   * Drupal\Core\Entity\EntityTypeManagerInterface definition.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * GuzzleHttp\ClientInterface definition.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $httpClient;

  /**
   * Regex field patterns used for parsing values from Drupal module page.
   */
  const FILED_PATTERNS = [
    'version' => '/View release notes\">(.*)?<\/a>/',
    'sites' => '/project\/usage\/{module}.*?\<strong\>(.*)?<\/strong>/',
    'issues' => '/project\/issues\/{module}\?categories\=All\">(.*)?<\/a>/',
    'bugs' => '/project\/issues\/{module}\?categories\=1\">(.*)?<\/a>/',
  ];

  /**
   * Constructs a new ModuleChecker object.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, ClientInterface $http_client) {
    $this->entityTypeManager = $entity_type_manager;
    $this->httpClient = $http_client;
  }

  /**
   * Cron callback which update module info entities data.
   *
   * @throws \Drupal\Component\Plugin\Exception\InvalidPluginDefinitionException
   * @throws \Drupal\Component\Plugin\Exception\PluginNotFoundException
   * @throws \Drupal\Core\Entity\EntityStorageException
   */
  public function cron() {
    $modules = $this->entityTypeManager->getStorage('module_info')->loadMultiple();
    /** @var \Drupal\module_checker\Entity\ModuleInfo $module */
    foreach ($modules as $module) {
      $module_name = $this->getMachineName($module->label());
      if ($html = $this->getModulePageHtml($module_name)) {
        $issues = $this->parseModulePageValue('issues', $html, $module_name);
        $version = $this->parseModulePageValue('version', $html, $module_name);
        $sites = $this->parseModulePageValue('sites', $html, $module_name);
        $bugs = $this->parseModulePageValue('bugs', $html, $module_name);

        $module->set('version', $version ?? '');
        $module->set('sites', $sites ?? 0);
        $module->set('issues', $issues ?? 0);
        $module->set('bugs', $bugs ?? 0);
        $module->save();
      }
    }
  }

  /**
   * Return machine name.
   *
   * @param string $module_name
   *   Module name.
   *
   * @return array|string|string[]
   *   Return filterd module name.
   */
  public function getMachineName(string $module_name) {
    return str_replace(' ', '_', strtolower($module_name));
  }

  /**
   * Get module contrb page body.
   *
   * @param string $module_name
   *   Module name.
   *
   * @return false|string|void
   *   Html code of the page.
   */
  public function getModulePageHtml(string $module_name) {
    $uri = 'https://www.drupal.org/project/' . $this->getMachineName($module_name);
    try {
      $response = $this->httpClient->get($uri, ['headers' => ['Accept' => 'text/html']]);
      $data = (string) $response->getBody();
      if (!empty($data)) {
        return $data;
      }
    }
    catch (TransferException $e) {
      return FALSE;
    }
  }

  /**
   * Parse given value from page body.
   *
   * @param string $type
   *   Value type.
   * @param string $html
   *   Page html code.
   * @param string $module_name
   *   Module name.
   *
   * @return array|false|mixed|string|string[]|null
   *   Return parsed value.
   */
  private function parseModulePageValue(string $type, string $html, string $module_name) {
    $matches = [];
    if (!isset(self::FILED_PATTERNS[$type])) {
      return FALSE;
    }

    $pattern = str_replace('{module}', $module_name, self::FILED_PATTERNS[$type]);
    if (preg_match($pattern, $html, $matches)) {
      if (!empty($matches[1])) {
        $value = $matches[1];
        // Filter founded value.
        switch ($type) {
          case 'sites':
            $value = str_replace(',', '', $value);
            break;

          case 'issues':
          case 'bugs':
            $value = preg_replace('/\D/', '', $value);
            break;
        }
        return $value;
      }
    }
    return FALSE;
  }

  /**
   * Return module info entity by machine name.
   *
   * @param string $module_name
   *   Module name.
   *
   * @return \Drupal\Core\Entity\EntityInterface[]
   *   Return module info entity or FALSE.
   *
   * @throws \Drupal\Component\Plugin\Exception\InvalidPluginDefinitionException
   * @throws \Drupal\Component\Plugin\Exception\PluginNotFoundException
   */
  public function moduleExists(string $module_name) {
    $machine_name = $this->getMachineName($module_name);
    $result = $this->entityTypeManager->getStorage('module_info')->loadByProperties(['machine_name' => $machine_name]);
    return $result ? reset($result) : FALSE;
  }

  /**
   * Get modules array.
   *
   * @param int $count
   *   Number of returned modules.
   *
   * @return \Drupal\Core\Entity\EntityInterface[]
   *   Array of module info entities.
   *
   * @throws \Drupal\Component\Plugin\Exception\InvalidPluginDefinitionException
   * @throws \Drupal\Component\Plugin\Exception\PluginNotFoundException
   */
  public function getModulesInfoData(int $count = 10) {
    $ids = \Drupal::entityQuery('module_info')
      ->condition('version', '', '<>')
      ->range(0, $count)
      ->execute();
    return $this->entityTypeManager->getStorage('module_info')->loadMultiple($ids);
  }

}
