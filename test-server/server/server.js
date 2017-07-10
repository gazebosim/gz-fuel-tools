'use strict'

const fs = require('fs')
const cors = require('cors')
const dotenv = require('dotenv')

// simple express server
const express = require('express')
const app = express()
const morgan = require('morgan')
const bodyParser = require('body-parser')

const util = require('util');

const models = require('./models')
const worlds = require('./worlds')

// dotenv.load()

const port = process.env.PORT || 8001

const corsOptions = {
  origin: [
    'http://localhost:8080',
    'http://localhost:5000',
    'http://localhost:4000'],
  credentials: true
}

app.use(cors(corsOptions))

// parse application/json
app.use(bodyParser.json())

app.use(morgan('combined'))
app.use(express.static('public'));

app.get('/', function(req, res) {
  res.sendfile('./public/index.html');
})


models.setRoutes(app)
worlds.setRoutes(app)

// Expose app
exports = module.exports = app;

// ssl and https
let httpServer = null

const useHttps = false
if(useHttps) {
  const privateKey  = fs.readFileSync(__dirname + '/key.pem', 'utf8')
  const certificate = fs.readFileSync(__dirname + '/key-cert.pem', 'utf8')
  httpServer = require('https').Server({
    key: privateKey, cert: certificate
  }, app)
}
else {
  httpServer = require('http').Server(app)
}

httpServer.listen(port)
