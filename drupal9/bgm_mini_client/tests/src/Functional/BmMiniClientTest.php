<?php

namespace Drupal\Tests\bgm_mini_client\Functional;

use Drupal\field\Entity\FieldConfig;
use Drupal\field\Entity\FieldStorageConfig;
use Drupal\Tests\BrowserTestBase;
use Drupal\Core\Url;


/**
 * Tests BGM Mini remote login.
 *
 * @group bgm_mini_client
 */
class BmMiniClientTest extends BrowserTestBase {

  /**
   * Modules to install.
   *
   * @var array
   */
  protected static $modules = [
    'field',
    'system',
    'text',
    'user',
    'bgm_mini_client',
    'bgm_mini_client_test',
  ];

  /**
   * {@inheritdoc}
   */
  protected $defaultTheme = 'stark';

  /**
   * Set up test environment.
   */
  protected function setUp(): void {
    parent::setUp();

    // Create user external id field.
    $field_name = 'field_external_id';
    FieldStorageConfig::create([
      'field_name' => $field_name,
      'entity_type' => 'user',
      'type' => 'text',
    ])->save();

    FieldConfig::create([
      'field_name' => $field_name,
      'entity_type' => 'user',
      'label' => 'External user id',
      'bundle' => 'user',
    ])->save();

    // Create not admin user and log in.
    $this->webUser = $this->drupalCreateUser([
      'access content',
    ]);

    // Set random value for external id.
    $this->random_uuid = rand(100, 1000);
    $this->webUser->set($field_name, [['value' => $this->random_uuid]]);
    $this->webUser->save();

    // Log in
    $this->drupalLogin($this->webUser);
    // Set test remote endpoint url.
    $remote_login_uri = Url::fromUserInput('/remote_login_test', ['absolute' => TRUE])
      ->toString();
    $this->config('bgm_mini_client.settings')
      ->set('remote_login_uri', $remote_login_uri)
      ->save();
  }

  /**
   * Test remote user login.
   */
  public function testRemoteLogin() {
    /** @var \Drupal\bgm_mini_client\BgmMiniClientService $BgmMiniClient */
    $BgmMiniClient = \Drupal::service('bgm_mini_client.client_service');
    $redirect_url = $BgmMiniClient->getUserLoginUrl();
    $uuid = $this->webUser->get('field_external_id')->value;

    $this->drupalGet($redirect_url);
    $this->assertSession()->statusCodeEquals(200);
    $success_message = 'JWT decoded successfully uid:' . $uuid;
    $this->assertSession()->pageTextContainsOnce($success_message);
  }
}
