const readline = require('readline');
const migrate = require('./src/scripts/migration.js');
const runBenchmark = require('./src/scripts/benchmark.js');
const runRedisDemo = require('./src/scripts/redis.js');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

const showMenu = () => {
    console.log('\n---Real Estate NoSQL Lab ---');
    console.log('1.Doing migration (SQL -> MongoDB)');
    console.log('2.Running Benchmark');
    console.log('3.Redis (Key-Value)');
    console.log('0.Exit');

    rl.question('\n Choose option (0-3): ', async (answer) => {
        switch (answer.trim()) {
            case '1':
                await migrate();
                showMenu();
                break;
            case '2':
                await runBenchmark();
                showMenu();
                break;
            case '3':
                await runRedisDemo();
                showMenu();
                break;
            case '0':
                rl.close();
                process.exit(0);
                break;
            default:
                console.log('Choose another option');
                showMenu();
                break;
        }
    });
};

showMenu();