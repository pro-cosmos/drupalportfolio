<?php

// $Id: unisendresponder.module,v 1.2.2.1.2.3.2.8.2.36 2010/08/09 18:41:08 Wiseman Exp $

/**
 * @file
 * Unisendresponder module.
 */
define('UNISENDRESPONDER_URL', 'api.unisender.com'); //'api.unisendresponder.ru');

class UNISENDAPI {

  var $version = "1.0-beta";
  var $errorMessage;
  var $errorCode;
  var $api_key;

  function UNISENDAPI($unisend_apikey) {
    $this->api_key = $unisend_apikey;
  }

  function getLists() {
    return $this->unisendresponder_process_request("getLists", 'api_key=' . $this->api_key);
  }

  function listSubscribe($list_all, $email, $extra, $double_optin) {
    /* $data = 'api_key=' . $unisend_apikey . '&';
      $data .= 'list_ids=' . $list_all . '&';
      $data .= 'fields[email]=' . $email . '&';
      $data .= 'fields[Name]=' . $dop_fields . '&';
      $data .= 'double_optin=' . $double_optin ; //. '&';
      return $this->unisendresponder_process_request("subscribe", $data);
     */

    $UniSender = new UniSenderApi($this->api_key);
    return $UniSender->subscribe(array(
          'list_ids' => $list_all,
          'fields' => array('email' => $email, 'Name' => $extra['Name']),
          'double_optin' => $double_optin
        ));
  }

  function listUnsubscribe($unisend_apikey, $contact_type, $list, $contact) {
    // $unisend_apikey = $unisend_apikey . 'error';
    $data = array();
    $data = 'api_key=' . $unisend_apikey . '&';
    $data .= 'list_ids=' . $list . '&';
    $data .= 'contact_type=' . $contact_type . '&';
    $data .= 'contact=' . $contact . '&';
    return $this->unisendresponder_process_request("unsubscribe", $data);
  }

//временная заглушка для получения данных о подписчике. С запуском такой возможности будет переписана.
  function getUserInfo($unisend_apikey, $contact) {
    // $unisend_apikey = $unisend_apikey . 'error';
    $data = array();
    $data = 'api_key=' . $unisend_apikey . '&';
    $data .= 'fields[email]=' . $contact . '&';
    return $this->unisendresponder_process_request("subscribe", $data);
  }

  function userRegister($unisend_apikey, $email, $login, $options) {
    // $unisend_apikey = $unisend_apikey . 'error';
    $data = array();
    $data = 'api_key=' . $unisend_apikey . '&';
    $data .= 'email=' . $email . '&';
    $data .= 'login=' . $login;
    $data .= $options;
    return $this->unisendresponder_process_request("register", $data);
  }

  /**
   * Process a unisendresponder action request.
   *
   * @param $action
   *   The unisendresponder action string.
   * @param $data
   *   An associate array of the data to send with this action.
   * @return
   *   A structured array of the response.
   */
  function unisendresponder_process_request($action, $data) {

    if (variable_get('unisendresponder_server_ssl', FALSE) && variable_get('unisendresponder_server_ssl', FALSE) == 1) {
      $scheme = 'https';
    } else {
      $scheme = 'http';
    }

    //$url = variable_get('unisendresponder_server_location', '') . '/en/api/'. $action .'?format=json&api_key='. $data;
    $url = $scheme . '://' . UNISENDRESPONDER_URL . '/ru/api/' . $action . '?format=json';
    $headers = array('Content-Type' => 'application/x-www-form-urlencoded');

    //$response = drupal_http_request($url, $headers, 'POST', $data);
    $response = drupal_http_request($url, array('headers' => $headers, 'method' => 'POST', 'data' => $data));
    $response = _jsonDecode($response->data);
    if (variable_get('unisendresponder_debug', FALSE) && variable_get('unisendresponder_debug', FALSE) == 1) {
      watchdog('unisendresponder', 'Request: %request and Response: %response', array('%request' => $url, '%response' => print_r($response, TRUE)), WATCHDOG_DEBUG);
      watchdog('unisendresponder', 'data:  %data and url: %url', array('%data' => $data, '%url' => $url), WATCHDOG_DEBUG);
      watchdog('unisendresponder', 'data_r:  %data and url: %url', array('%data' => print_r($data, TRUE), '%url' => $url), WATCHDOG_DEBUG);
    }
    return $response;
  }

}

/* * ******************* **************** */

function _jsonDecode($data) {
  //Thanks to www at walidator dot info
  //(http://www.php.net/manual/en/function.json-decode.php#91216)
  /*  if (!function_exists('json_decode') ) {
    function json_decode($json) {
    $comment = false;
    $out = '$x=';

    for ($i=0; $i<strlen($json); $i++) {
    if (!$comment) {
    if ($json[$i] == '{')        $out .= ' array(';
    else if ($json[$i] == '}')    $out .= ')';
    else if ($json[$i] == ':')    $out .= '=>';
    else                         $out .= $json[$i];
    }
    else $out .= $json[$i];
    if ($json[$i] == '"')    $comment = !$comment;
    }
    eval($out . ';');
    return $x;
    }
    } */
  //другой вариант
  /**
   * json_decode for < PHP 5
   */
  if (!function_exists('json_decode')) {

    function json_decode($v) {
      if ($v{0} == '"') {
        $v = drupal_substr($v, 1, -1);
      } elseif ($v{0} == '{') {
        $var = explode(",", drupal_substr($v, 1, -1)); # changed from drupal_substr($v, 1, -2), since it was chopping off the final quote
        $v = array();
        foreach ($var as $value) {
          $va = explode(":", $value);
          # added the following condition to remove quotes from the key
          if ($va[0]{0} == '"') {
            $va[0] = drupal_substr($va[0], 1, -1);
          }
          $v[$va[0]] = json_decode($va[1]);
        }
      } elseif ($v{0} == '[') {
        $var = explode(",", drupal_substr($v, 1, -2));
        $v = array();
        foreach ($var as $value) {
          $v[] = json_decode($va[0]);
        }
      }
      return $v;
    }

  }

  return json_decode($data);
}

/**
 * API UniSender
 *
 * @see http://www.unisender.com/ru/help/api/
 * @version 1.3
 */
class UniSenderApi {

  /**
   * @var string
   */
  protected $ApiKey;

  /**
   * @var string
   */
  protected $Encoding = 'UTF8';

  /**
   * @var int
   */
  protected $RetryCount = 0;

  /**
   * @var float
   */
  protected $Timeout;

  /**
   * @var bool
   */
  protected $Compression = false;

  /**
   * @param string $ApiKey
   * @param string $Encoding
   * @param int $RetryCount
   */
  function __construct($ApiKey, $Encoding = 'UTF8', $RetryCount = 4, $Timeout = null, $Compression = false) {
    $this->ApiKey = $ApiKey;

    if (!empty($Encoding)) {
      $this->Encoding = $Encoding;
    }

    if (!empty($RetryCount)) {
      $this->RetryCount = $RetryCount;
    }

    if (!empty($Timeout)) {
      $this->Timeout = $Timeout;
    }

    if ($Compression) {
      $this->Compression = $Compression;
    }
  }

  /**
   * @param string $Name
   * @param array $Arguments
   * @return string
   */
  function __call($Name, $Arguments) {
    if (!is_array($Arguments) || empty($Arguments)) {
      $Params = array();
    } else {
      $Params = $Arguments[0];
    }

    return $this->callMethod($Name, $Params);
  }

  /**
   * @param array $Params
   * @return string
   */
  function subscribe($Params) {
    $Params = (array) $Params;

    if (empty($Params['request_ip'])) {
      $Params['request_ip'] = $this->getClientIp();
    }

    return $this->callMethod('subscribe', $Params);
  }

  /**
   * @param string $JSON
   * @return mixed
   */
  protected function decodeJSON($JSON) {
    return json_decode($JSON);
  }

  /**
   * @return string
   */
  protected function getClientIp() {
    $Result = '';

    if (!empty($_SERVER["REMOTE_ADDR"])) {
      $Result = $_SERVER["REMOTE_ADDR"];
    } else if (!empty($_SERVER["HTTP_X_FORWARDED_FOR"])) {
      $Result = $_SERVER["HTTP_X_FORWARDED_FOR"];
    } else if (!empty($_SERVER["HTTP_CLIENT_IP"])) {
      $Result = $_SERVER["HTTP_CLIENT_IP"];
    }

    if (preg_match('/([0-9]|[0-9][0-9]|[01][0-9][0-9]|2[0-4][0-9]|25[0-5])(\.([0-9]|[0-9][0-9]|[01][0-9][0-9]|2[0-4][0-9]|25[0-5])){3}/', $Result, $Match)) {
      return $Match[0];
    }

    return $Result;
  }

  /**
   * @param string $Value
   * @param string $Key
   */
  protected function iconv(&$Value, $Key) {
    $Value = iconv($this->Encoding, 'UTF8//IGNORE', $Value);
  }

  /**
   * @param string $Value
   * @param string $Key
   */
  protected function mb_convert_encoding(&$Value, $Key) {
    $Value = mb_convert_encoding($Value, 'UTF8', $this->Encoding);
  }

  /**
   * @param string $MethodName
   * @param array $Params
   * @return array
   */
  protected function callMethod($MethodName, $Params = array()) {
    if ($this->Encoding != 'UTF8') {
      if (function_exists('iconv')) {
        array_walk_recursive($Params, array($this, 'iconv'));
      } else if (function_exists('mb_convert_encoding')) {
        array_walk_recursive($Params, array($this, 'mb_convert_encoding'));
      }
    }

    $Url = $MethodName . '?format=json';

    if ($this->Compression) {
      $Url .= '&api_key=' . $this->ApiKey . '&request_compression=bzip2';
      $Content = bzcompress(http_build_query($Params));
    } else {
      $Params = array_merge((array) $Params, array('api_key' => $this->ApiKey));
      $Content = http_build_query($Params);
    }

    $ContextOptions = array(
      'http' => array(
        'method' => 'POST',
        'header' => 'Content-type: application/x-www-form-urlencoded',
        'content' => $Content,
      )
    );

    if ($this->Timeout) {
      $ContextOptions['http']['timeout'] = $this->Timeout;
    }

    $RetryCount = 0;
    $Context = stream_context_create($ContextOptions);

    do {
      $Host = $this->getApiHost($RetryCount);
      $Result = file_get_contents($Host . $Url, false, $Context);
      $RetryCount++;
    } while ($Result === false && $RetryCount < $this->RetryCount);

    return $Result;
  }

  /**
   * @param int $RetryCount
   * @return string
   */
  protected function getApiHost($RetryCount = 0) {
    if ($RetryCount % 2 == 0) {
      return 'http://api.unisender.com/ru/api/';
    } else {
      return 'http://www.api.unisender.com/ru/api/';
    }
  }

}