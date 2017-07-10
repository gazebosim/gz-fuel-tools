'use strict'

const express = require('express')
const router = express.Router();

let worlds = []

let world0 = {
  name: 'mars',
  description: 'red planet',
  tags: [{
    id: '222',
    name: 'mars_tag_name'
  }]
}

worlds.push(world0)

exports.setRoutes = function (app) {
  router.get('/worlds', (req, res) => {
    res.jsonp(worlds);
  })

  router.get('/worlds/:id', (req, res) => {
    let r = null
    let idx = parseInt(req.id)
    if (idx < worlds.length)
      r = worlds[idx]
    res.jsonp(r);
  })


  router.post('/worlds', (req, res) => {
    const m = req.body
    worlds.push(m)
    res.jsonp(m);
  })


  router.delete('/worlds/:id', (req, res) => {
    let idx = parseInt(req.id)
    let r = null
    if (idx < worlds.length) {
      r = JSON.parse(JSON.stringify(worlds[idx]))
      worlds[idx] = null
    }
    res.jsonp(r);
  })

  /// param for resource name
  router.param('id', (req, res, next, id) => {
    req.id = id
    next()
  })

  app.use('/1.0', router)
}
