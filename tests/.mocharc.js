var base_time = 4000;
if (process.env.LEDGER_LIVE_HARDWARE) {
  base_time = 0;
}

module.exports = {
  timeout: parseInt(process.env.GEN_TIME_LIMIT || base_time) * 2
};
console.log("Config file loaded.");
console.log(module.exports);
