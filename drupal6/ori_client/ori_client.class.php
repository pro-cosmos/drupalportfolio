<?php


class OriClient {

    protected static $instance; // object instance

    public static function init() {

        if (is_null(self::$instance)) {
            self::$instance = new self;
        }

        return self::$instance;
    }

    public static function version() {
        return self::xmlrpc(
                        'ori_server.version'
        );
    }

    public static function _get_secretcode(){
       return  (string)variable_get('constant_ori_client_secretcode', '');
    }
    public static function _get_xmlrpcserver(){
       return  variable_get('constant_ori_client_xmlrpcserver', '');
    }


    public static function Auth(){
        $secretcode = self::_get_secretcode();
        if($secretcode)
        {
          $response = self::unpack(xmlrpc(self::_get_xmlrpcserver(),'ori_server.auth', $secretcode));

          if($response)
              return true; //ok
            else {
              $error = xmlrpc_error();
              self::message('Cant connect to XMLRPC server! Error: ' . $error->message);
            }
        }
        else
            self::message('Empty account code!');

    return false;
    }

    private static function message($m){
       drupal_set_message('OriClient: '. $m);
       watchdog('OriClient', 'OriClient: '. $m);
    }

    public static function xmlrpc() {

        if (self::Auth()) {
            $args = func_get_args();
            $response = call_user_func_array('xmlrpc', array_merge(array(self::_get_xmlrpcserver()), $args));

            //if($args[0] = 'ori_server.user_load')
              //return $response ;

            return self::unpack($response);
        }


        return false;
    }

    public static function get_state($country) {

        return self::xmlrpc(
                        'ori_server.get_state', (string) $country
        );
    }

    public static function get_country($arg = array()) {
        return self::xmlrpc(
                        'ori_server.get_country'
        );
    }

    public static function get_postindex($country, $search) {

        return self::xmlrpc(
                        'ori_server.get_postindex', (string) $country, (string) $search
        );
    }

    public static function get_town($country, $search, $state) {

        return self::xmlrpc(
                        'ori_server.get_town', (string) $country, (string) $search, (string) $state
        );
    }

    public static function get_district($country, $search, $state) {

        return self::xmlrpc(
                        'ori_server.get_district', (string) $country, (string) $search, (string) $state
        );
    }

    public static function get_place($country, $search, $state, $area) {

        return self::xmlrpc(
                        'ori_server.get_place', (string) $country, (string) $search, (string) $state, (string) $area
        );
    }

    /**
     *
     * @param type $arg
     * @return type
     */
    public static function validateUser($values) {
         if ($values) {
            return self::xmlrpc(
                            'ori_server.user_registr_validate',  self::pack($values)
            );
         }

        return false;
    }

    /**
     *
     * @param type $arg
     * @return type
     */
    public static function regnumberUser($regnumber) {

       $regnumber = trim($regnumber);
        if ($regnumber) {

            return self::xmlrpc(
                            'ori_server.user_load', 'regnumber', array($regnumber)
                            );
        }

        return false;
    }

    /**
     *
     * @param type $arg
     * @return type
     */
    public static function loadUser($hash) {

        if ($hash) {
            return self::xmlrpc(
                            'ori_server.user_load', 'hash', array($hash)
            );
        }

        return false;
    }

    /**
     * провера данных и регистрация пользователя
     */
    public static function confirmUser($confirmCode) {
        if ($confirmCode) {
            return self::xmlrpc(
                            'ori_server.user_confirm', (string) $confirmCode
            );
        }

        return array('result' => false);
    }

    /**
     * провера данных и регистрация пользователя
     */
    public static function registerUser($values) {

        if ($values) {
            return self::xmlrpc(
                            'ori_server.user_registr', self::pack($values)
            );
        }
    }

    /**
     *
     * @param type $arg
     * @return type
     */
    protected static function unpack($r) {
        return json_decode($r, 1);
    }

    /**
     *
     * @param type $r
     * @return type
     */
    protected static function pack($r) {
        return json_encode($r);
    }

/**
 *
 * @param type $domain
 * @return type
 */
    public static function loadUserSite($domain) {
        if ($domain) {
            return self::xmlrpc(
                            'ori_server.partner_site_load', (string)$domain
            );
        }
        return false;
    }

}