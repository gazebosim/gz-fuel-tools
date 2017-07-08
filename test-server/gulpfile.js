'use strict';

var gulp = require('gulp');
var nodemon = require('gulp-nodemon');
var mocha = require('gulp-mocha');

gulp.task('default', ['serve'], function () {
});

gulp.task('serve', ['nodemon'], function () {
});

gulp.task('nodemon', function (cb) {

  var started = false;

  return nodemon({
    script: 'server/server.js',
    watch: ['server/**/*.*']
  }).on('start', function () {
    console.log('start nodemon')
    // to avoid nodemon being started multiple times
    // thanks @matthisk
    if (!started) {
      cb();
      started = true;
    }
  });
});

gulp.task('set-test-env', function () {
  return process.env.NODE_ENV = 'test';
});

gulp.task('test', ['set-test-env'], function() {
  return gulp.src(['test/*.js'], {read: false})
    .pipe(mocha({
      reporter: 'spec'
    }))
    .once('end', function () {
      process.exit();
    });
});

