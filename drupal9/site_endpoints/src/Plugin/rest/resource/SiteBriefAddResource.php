<?php

namespace Drupal\site_endpoints\Plugin\rest\resource;

use Drupal\Core\File\Exception\FileException;
use Drupal\Core\File\FileSystemInterface;
use Drupal\node\Entity\Node;
use Drupal\rest\ModifiedResourceResponse;
use Drupal\Component\Render\PlainTextOutput;
use Drupal\file\Entity\File;
use Symfony\Component\HttpKernel\Exception\UnprocessableEntityHttpException;
use Symfony\Component\Mime\MimeTypeGuesserInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\Exception\HttpException;
use Drupal\file\Plugin\rest\resource\FileUploadResource;

/**
 * File upload resource.
 *
 * This is implemented as a field-level resource for the following reasons:
 *   - Validation for uploaded files is tied to fields (allowed extensions, max
 *     size, etc..).
 *   - The actual files do not need to be stored in another temporary location,
 *     to be later moved when they are referenced from a file field.
 *   - Permission to upload a file can be determined by a users field level
 *     create access to the file field.
 *
 * @RestResource(
 *   id = "site:brief_add",
 *   label = @Translation("Site Brief Add"),
 *   serialization_class = "Drupal\file\Entity\File",
 *   uri_paths = {
 *     "create" = "/site/brief_add/{entity_type_id}/{bundle}/{field_name}"
 *   }
 * )
 */
class SiteBriefAddResource extends FileUploadResource {

  /**
   * Creates a file from an endpoint.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request.
   * @param string $entity_type_id
   *   The entity type ID.
   * @param string $bundle
   *   The entity bundle. This will be the same as $entity_type_id for entity
   *   types that don't support bundles.
   * @param string $field_name
   *   The field name.
   *
   * @return \Drupal\rest\ModifiedResourceResponse
   *   A 201 response, on success.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\HttpException
   *   Thrown when temporary files cannot be written, a lock cannot be acquired,
   *   or when temporary files cannot be moved to their new location.
   */
  public function post(Request $request, $entity_type_id, $bundle, $field_name) {
    $files = [];
    foreach ($request->files as $file) {
      /** @var \Symfony\Component\HttpFoundation\File\UploadedFile $file */
      $files[] = $this->uploadFile($file, $entity_type_id, $bundle, $field_name);
    }

    /** @var \Drupal\node\NodeInterface $node */
    $node = $this->createBriefNode($request, $files);

    $result = ['status' => $node->id()];
    // 201 Created responses return the newly created entity in the response
    // body. These responses are not cacheable, so we add no cacheability
    // metadata here.
    return new ModifiedResourceResponse($result, 201);
  }

  /**
   * Create Brief node from request data.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request.
   * @param array $files
   *   Array of uploaded files.
   *
   * @return \Drupal\node\Entity\Node|false
   *   Return Brief node or false.
   *
   * @throws \Drupal\Core\Entity\EntityStorageException
   * @throws \Drupal\Core\TypedData\Exception\MissingDataException
   * @throws \Drupal\Core\TypedData\Exception\ReadOnlyException
   */
  private function createBriefNode(Request $request, array $files = []) {
    $brief = $request->request->all();
    if (empty($brief)) {
      return FALSE;
    }

    $node = Node::create(['type' => 'brief']);

    $node->field_brief_email->setValue($brief['email'] ?? '');
    $node->field_brief_phone->setValue($brief['phone'] ?? '');
    $node->field_brief_fio->setValue($brief['fullname'] ?? '');
    $node->field_brief_company->setValue($brief['company'] ?? '');
    $node->field_brief_connection->setValue($brief['connection'] ?? 'Email');
    $body = [
      'summary' => '',
      'value' => $brief['body'] ?? '',
      'format' => 'full_html',
    ];
    $node->body = $body;
    // Update brief type field.
    if (!empty($brief['brief_type'])) {
      foreach (explode(',', $brief['brief_type']) as $key => $eid) {
        $node->field_brief_type->set($key, $eid);
      }
    }
    // Update brief service field.
    if (!empty($brief['brief_service'])) {
      foreach (explode(',', $brief['brief_service']) as $key => $eid) {
        $node->field_brief_service->set($key, $eid);
      }
    }
    // Save attached files.
    if (!empty($files)) {
      foreach ($files as $key => $file) {
        $node->field_brief_files->set($key, $file);
      }
    }

    $node->save();
    return $node;
  }

  /**
   * Handling of file upload.
   *
   * @param \Symfony\Component\HttpFoundation\File\UploadedFile $file
   *   File to upload.
   * @param string $entity_type_id
   *   The entity type ID.
   * @param string $bundle
   *   The entity bundle. This will be the same as $entity_type_id for entity
   *   types that don't support bundles.
   * @param string $field_name
   *   The field name.
   *
   * @return \Drupal\Core\Entity\ContentEntityBase|\Drupal\Core\Entity\EntityBase|\Drupal\Core\Entity\EntityInterface|\Drupal\file\Entity\File
   *   Return created file entity or throw exception.
   *
   * @throws \Drupal\Core\Entity\EntityStorageException
   */
  private function uploadFile(\Symfony\Component\HttpFoundation\File\UploadedFile $file, $entity_type_id, $bundle, $field_name) {
    $filename = $file->getClientOriginalName();
    $field_definition = $this->validateAndLoadFieldDefinition($entity_type_id, $bundle, $field_name);

    $destination = $this->getUploadLocation($field_definition->getSettings());

    // Check the destination file path is writable.
    if (!$this->fileSystem->prepareDirectory($destination, FileSystemInterface::CREATE_DIRECTORY)) {
      throw new HttpException(500, 'Destination file path is not writable');
    }

    $validators = $this->getUploadValidators($field_definition);

    $prepared_filename = $this->prepareFilename($filename, $validators);

    // Create the file.
    $file_uri = "{$destination}/{$prepared_filename}";

    // $temp_file_path = $this->streamUploadData($request);
    $temp_file_path = $file->getRealPath();

    $file_uri = $this->fileSystem->getDestinationFilename($file_uri, FileSystemInterface::EXISTS_RENAME);

    // Lock based on the prepared file URI.
    $lock_id = $this->generateLockIdFromFileUri($file_uri);

    if (!$this->lock->acquire($lock_id)) {
      throw new HttpException(503, sprintf('File "%s" is already locked for writing', $file_uri), NULL, ['Retry-After' => 1]);
    }

    // Begin building file entity.
    $file = File::create([]);
    $file->setOwnerId($this->currentUser->id());
    $file->setFilename($prepared_filename);
    if ($this->mimeTypeGuesser instanceof MimeTypeGuesserInterface) {
      $file->setMimeType($this->mimeTypeGuesser->guessMimeType($prepared_filename));
    }
    else {
      $file->setMimeType($this->mimeTypeGuesser->guess($prepared_filename));
      @trigger_error('\Symfony\Component\HttpFoundation\File\MimeType\MimeTypeGuesserInterface is deprecated in drupal:9.1.0 and is removed from drupal:10.0.0. Implement \Symfony\Component\Mime\MimeTypeGuesserInterface instead. See https://www.drupal.org/node/3133341', E_USER_DEPRECATED);
    }
    $file->setFileUri($temp_file_path);
    // Set the size. This is done in File::preSave() but we validate the file
    // before it is saved.
    $file->setSize(@filesize($temp_file_path));

    // Validate the file against field-level validators first while the file is
    // still a temporary file. Validation is split up in 2 steps to be the same
    // as in _file_save_upload_single().
    // For backwards compatibility this part is copied from ::validate() to
    // leave that method behavior unchanged.
    // @todo Improve this with a file uploader service in
    //   https://www.drupal.org/project/drupal/issues/2940383
    $errors = file_validate($file, $validators);

    if (!empty($errors)) {
      $message = "Unprocessable Entity: file validation failed.\n";
      $array = array_map([PlainTextOutput::class, 'renderFromHtml'], $errors);
      $message .= implode("\n", $array);

      throw new UnprocessableEntityHttpException($message);
    }

    $file->setFileUri($file_uri);
    // Move the file to the correct location after validation. Use
    // FileSystemInterface::EXISTS_ERROR as the file location has already been
    // determined above in FileSystem::getDestinationFilename().
    try {
      $this->fileSystem->move($temp_file_path, $file_uri, FileSystemInterface::EXISTS_ERROR);
    }
    catch (FileException $e) {
      throw new HttpException(500, 'Temporary file could not be moved to file location');
    }

    // Second step of the validation on the file object itself now.
    $this->resourceValidate($file);

    $file->save();

    $this->lock->release($lock_id);

    return $file;
  }

  /**
   * Streams file upload data to temporary file and moves to file destination.
   *
   * @return string
   *   The temp file path.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\HttpException
   *   Thrown when input data cannot be read, the temporary file cannot be
   *   opened, or the temporary file cannot be written.
   */
  protected function streamUploadData(Request $request) {
    if ($request->getContentType() === 'bin_multipart') {
      $fileKey = 'file';
      /** @var \Symfony\Component\HttpFoundation\File\UploadedFile $file */
      $file = $request->files->get($fileKey);

      return $file->getRealPath();
    }
  }

  /**
   * Validates and extracts the filename from the Content-Disposition header.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The request object.
   *
   * @return string
   *   The filename extracted from the header.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\BadRequestHttpException
   *   Thrown when the 'Content-Disposition' request header is invalid.
   */
  protected function validateAndParseContentDispositionHeader(Request $request) {
    if ($request->getContentType() === 'bin_multipart') {
      $fileKey = 'file';
      /** @var \Symfony\Component\HttpFoundation\File\UploadedFile $file */
      $file = $request->files->get($fileKey);

      return basename($file->getClientOriginalName());
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function getBaseRouteRequirements($method) {
    $requirements = parent::getBaseRouteRequirements($method);

    // Add the content type format access check. This will enforce that all
    // incoming requests can only use the 'application/octet-stream'
    // Content-Type header.
    $requirements['_content_type_format'] = 'bin|bin_multipart';

    return $requirements;
  }

}
