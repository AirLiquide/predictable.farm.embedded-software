'use strict'

const GREEN = 1
const RED = 2
const ORANGE = 3

let sleep = (delay) => {
  delay += new Date().getTime()
  while (new Date() < delay) { }
}

let led = (color) => {
  if (color == GREEN || color == RED || color == ORANGE) {
    exec('/home/root/userled.py ' + color)
  }
}

module.exports = {
  colors: {
    GREEN: GREEN,
    RED: RED,
    ORANGE: ORANGE
  },
  sleep: sleep,
  led: led
}
