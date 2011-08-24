<?php

/**
* @file
* FeedsZoteroFetcher
*/

/**
* Fetches data via HTTP.
*/
class FeedsZoteroFetcher extends FeedsHTTPFetcher {
   
   /**
   * Expose source form.
   */
   public function sourceForm($source_config) {
      $form = array();
      $form['zotero_type'] = array(
         '#type' => 'radios',
         '#title' => t('What type of Zotero library is this?'),
         '#options' => array('group' => 'Group', 'user' => 'User'),
         '#default_value' => isset($source_config['zotero_type']) ? $source_config['zotero_type'] : '',
         '#maxlength' => NULL,
         '#required' => TRUE,
         );
      $form['zotero_id'] = array(
         '#type' => 'textfield',
         '#title' => t('Zotero ID'),
         '#description' => t('Enter the numeric id of your zotero user or group. To find your 
            group id, navigate to your groups\'s settings page and the numeric part of the URL 
            is you group id. Your userID is in
            <a href="https://www.zotero.org/settings/keys">Feeds/API section</a> of your zotero settings.'),
         '#default_value' => isset($source_config['zotero_id']) ? $source_config['zotero_id'] : '',
         '#maxlength' => NULL,
         '#required' => TRUE,
         );
      $form['api_key'] = array(
         '#type' => 'textfield',
         '#title' => t('Zotero API key'),
         '#description' => t('If your group or user account is private, you will need to enter 
            your api key to be allowed to access the library.  Your api key is in
            <a href="https://www.zotero.org/settings/keys">Feeds/API section</a> of your zotero settings.'),
         '#default_value' => isset($source_config['api_key']) ? $source_config['api_key'] : '',
         '#maxlength' => NULL,
         );
      $form['source'] = array(
         '#type' => 'hidden',
         '#title' => t('URL'),
         '#disabled' => TRUE,
         '#description' => t('Enter a feed URL.'),
         '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
         '#maxlength' => NULL,
         );
      if ( ! empty($source_config['source'])) {
         $form['source'] = array(
            '#type' => 'markup',
            '#value' => '<p><label>URL</label>: ' . $source_config['source'] . '</p>',
            );
      }
      return $form;
   }
   
   /**
   * Override parent::sourceFormValidate().
   */
   public function sourceFormValidate(&$values) {
      // We need to construct a url that looks like this: 
      // https://api.zotero.org/groups/98765/items
      // or this:
      // https://api.zotero.org/users/12345/items
      $zotType = $values['zotero_type'] == 'group' ? 'groups' : 'users';
      
      if (! is_numeric($values['zotero_id'])) {
         $form_key = 'feeds][' . get_class($this) . '][zotero_id';
         form_set_error($form_key, t('The zotero id must be numeric.'));
      }
      
      $values['source'] = sprintf('https://api.zotero.org/%s/%d/items', $zotType, $values['zotero_id']);
      $values['source'] .= '?content=json';
      // TODO add the api key to the source
      
      // TODO test the accessibility of the API call.
      
      if (!feeds_valid_url($values['source'], TRUE)) {
         $form_key = 'feeds][' . get_class($this) . '][source';
         form_set_error($form_key, t('The URL %source is invalid.', array('%source' => $values['source'])));
      }
   }
   
      /**
   * Override parent::configDefaults().
   */
   public function configDefaults() {
      $form = array();
      return $form; 
   }
   
   /**
   * Override parent::configForm().
   */
   public function configForm(&$form_state) {
      $form = array();
      return $form; 
   }

}
