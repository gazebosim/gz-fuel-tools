'use strict'

const express = require('express')
const router = express.Router();
const multer = require('multer')
let upload = multer()

let models = []

let model0 = {
  name: 'car',
  uuid: '1234-abcd',
  description: 'I am a car',
  tags: [{
    id: '111',
    name: 'car_tag_name'
  }],
  images: [{
    id: '222',
    url: 'car_image_url'
  }],
  likes: 3,
  filesize: 1024,
  downloads: 5,
  upload_date: '2012-04-23T18:25:43.511Z',
  modify_date: '2012-04-23T18:25:43.511Z',
  category: 'transportation'
}

models.push(model0)

exports.setRoutes = function (app) {
  router.get('/models', (req, res) => {
    res.jsonp(models)
  })

  router.get('/models/:id', (req, res) => {
    let r = null
    let idx = parseInt(req.id)
    if (idx < models.length)
      r = models[idx]
    res.jsonp(r)
  })


  router.post('/models', upload.single('data'), (req, res) => {
    // meta data
    const m = req.body
    models.push(m)

    console.log('body :' + JSON.stringify(m));

    // file will be in req.file
    if (req.file)
      console.log('upload success!')

    res.jsonp(m)
  })


  router.delete('/models/:id', (req, res) => {
    let idx = parseInt(req.id)
    let r = null
    if (idx < models.length) {
      r = JSON.parse(JSON.stringify(models[idx]))
      models[idx] = null
    }
    res.jsonp(r)
  })

  /// param for resource name
  router.param('id', (req, res, next, id) => {
    req.id = id
    next()
  })

  app.use('/1.0', router)
}
