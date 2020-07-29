// routes/index.js

module.exports = function(app, Ccuser)
{
    var tzoffset = (new Date()).getTimezoneOffset() * 60000;
    // login
    app.post('/cc/adminlogin', function(req,res){
        if(req.body.id == 'admin' && req.body.password == 'rghiri'){
            res.json({result:"access granted"});
        }else{
            res.json({result:"access denied"});
        }
    });
    
    // handle gold minus
    app.post('/cc/handlegoldminus', function(req,res){
        var min = -1;
        Ccuser.find({gold:{$lt:min}}, function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            var count = 0;
            var ids = "";
            for(var i = 0; i < users.length; i++){
                users[i].gold = 777;
                ids += users[i].id;
                users[i].save(function(err){
                    if(err){
                        console.error("handle odd gold err: " + err);
                    }
                    count++;
                    if(count >= users.length){
                        res.json({result:ids});
                    }
                });
            }
        });
    });
    
    // find tree minus
    app.post('/cc/handletreeminus', function(req,res){
        var min = -1;
        Ccuser.find({tree:{$lt:min}}, function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            var count = 0;
            var ids = "";
            for(var i = 0; i < users.length; i++){
                users[i].tree = 777;
                ids += users[i].id;
                users[i].save(function(err){
                    if(err){
                        console.error("handle odd gold err: " + err);
                    }
                    count++;
                    if(count >= users.length){
                        res.json({result:ids});
                    }
                });
            }
        });
    });
    
    // handle odd gold
    app.post('/cc/handleoddgold', function(req,res){
        var min = -1;
        var max = 1000000;
        Ccuser.find({gold:{$gt:max}}, function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            var count = 0;
            var ids = "";
            for(var i = 0; i < users.length; i++){
                users[i].gold = 777;
                ids += users[i].id;
                users[i].save(function(err){
                    if(err){
                        console.error("handle odd gold err: " + err);
                    }
                    count++;
                    if(count >= users.length){
                        res.json({result:ids});
                    }
                });
            }
        });
    });
    
    // handle odd tree
    app.post('/cc/handleoddtree', function(req,res){
        var min = -1;
        var max = 1000000;
        Ccuser.find({tree:{$gt:max}}, function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            
            var count = 0;
            var ids = "";
            for(var i = 0; i < users.length; i++){
                users[i].tree = 777;
                ids += users[i].id;
                users[i].save(function(err){
                    if(err){
                        console.error("handle odd tree err: " + err);
                    }
                    count++;
                    if(count >= users.length){
                        res.json({result:ids});
                    }
                });
            }
        });
    });
    
    // handle odd trophy
    app.post('/cc/handleoddtrophy', function(req,res){
        var min = -1;
        var max = 1000000;
        Ccuser.find({trophy:{$gt:max}}, function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            var count = 0;
            var ids = "";
            for(var i = 0; i < users.length; i++){
                users[i].trophy = 1000;
                ids += users[i].id + "_";
                users[i].save(function(err){
                    if(err){
                        console.error("handle odd gold err: " + err);
                    }
                    count++;
                    if(count >= users.length){
                        res.json({result:ids});
                    }
                });
            }
        });
    });
    // find match
    app.post('/cc/findmatch', function(req,res){
        var min = req.body.trophy - 500;
//        var max = req.body.trophy + 1000;
//        console.log("find match for trophy: " + req.body.trophy);
//        {$gt:min, $lt:max}
//        Ccuser.find({ $and : [{trophy:{$gt:min}}, {id : {$ne : req.body.id}}, {deck : {$ne : "_"}}, {inventory : {$ne : "_"}}, {deck : {$ne : null}}, {inventory : {$ne : null}}] }
        Ccuser.find({trophy:{$gt:min}}
//            {trophy:{$gt:min, $lt:max}}
                    , function(err, users){
            if(err) return res.status(500).send({error: 'database failure'});
            if(users.length == 0){
                return res.status(500).send({error: 'user not found'});
            }
            
            var now = new Date(Date.now() - tzoffset).toISOString();
            var data = {time: now};//'{"time":' + now;
            var user = users[Math.floor((Math.random() * users.length))]
//            console.log("found user for match: " + user.id);
            data.id = user.id;
            data._id = user._id;
            data.name = user.name;
            data.buildings = user.buildings;
            data.deck = user.deck;
            data.hdck = user.hdck;
            data.gold = user.gold;
            if(data.gold < 0){
                data.gold = 0;
            }
            var maxGold = 500;
            if(data.gold > maxGold){
                data.gold = maxGold;
            }
            data.tree = user.tree;
            if(data.tree < 0){
                data.tree = 0;
            }
            var maxTree = 500;
            if(data.tree > maxTree){
                data.tree = maxTree;
            }
            
            if(user.shield_end){
                var endTime = new Date(user.shield_end);
                if(endTime.getTime() > (new Date(Date.now() - tzoffset)).getTime()){
                    data.gold = 0;
                    data.tree = 0;
                }
            }
//            if(user.id == "5232" || user.id == "2"){
//                console.log(now +" findmatch user.id + " + user.id+ "/" + req.body.id + "->" + user.buildings);
//            }
            data.trophy = user.trophy;
            res.json(data);
        });
    });
    
    
    // find top players
    app.post('/cc/findtopplayers', function(req,res){
//        db.posts.find( //query today up to tonight
//  {"created_on": {"$gte": new Date(2012, 7, 14), "$lt": new Date(2012, 7, 15)}})
        Ccuser
        .find({ $and : [{trophy: { $gt: 1000, $lt:100000 }}, {ban:0}]}, ['id', 'name', 'trophy'])
//        .select({})
        .sort({trophy : -1})
        .limit(Number(req.body.count))
        .exec(function(err, doc){
            if(err){
                console.log("find top players err: " + err);
                res.json({error: "failed to get top players"});
                return;
            }
//            console.log("find top players result: " + doc);
//            let max_price = doc[0].price;
            res.json({result: doc});
        });
    });
    
    
    // find ranking
    app.post('/cc/getrank', function(req,res){
//        console.log("get trohpy rank");
        Ccuser.countDocuments({ trophy: { $gt: req.body.trophy }} , function(err, result){
            if(err){
                console.log("err: " + err);
                res.json({error: "count failed"});
            }
            // count is the result
            res.json({result: result});
    //         age: trophy $gt: 17, $req.body.trophy         res.json({result: user.id});
        });
//        var leaderboard = Ccuser.find( {points: {$exists: true}} ).sort({field : -1}).limit(5).toArray();
//        console.log("leaderboard: " + leaderboard);
    });
    
    // add shield
    app.post('/cc/addshield', function(req, res){
//        console.log("addshield:" +  JSON.stringify(req.body));
        Ccuser.findOne({id: req.body.id}, function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            
            var now;
            if(user.shield_end){
                now = new Date(user.shield_end);
                if(now.getTime() < (new Date(Date.now() - tzoffset)).getTime()){
                    now = new Date(Date.now() - tzoffset);
                }
            }else{
                now = new Date(Date.now() - tzoffset);
            }
            
            now.setHours( now.getHours() + Number(req.body.hour));
            user.shield_end = now.toISOString();
//            console.log("user.shield_end " + user.shield_end);
            user.save(function(err){
                if(err){
                    console.error(err);
                    res.json({error: "save failed"});
                }else{
                    console.error("saved: " + user.shield_end);
                    res.json({result: user.shield_end});
                }
            });
        });
    });
    // add defence record
    app.post('/cc/adddefencerecord', function(req, res){
//        console.log("adddefencerecord:" +  JSON.stringify(req.body));
        Ccuser.findOne({id: req.body.id}, function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            
            if(!user.defence_record) user.defence_record = "";
            user.trophy += Number(req.body.trophy);
            if(user.trophy < 0){
                user.trophy = 0;
            }
            var gold = Number(req.body.gold);
            var tree = Number(req.body.tree);
            if(user.shield_end){
                var endTime = new Date(user.shield_end);
                if(endTime.getTime() > (new Date(Date.now() - tzoffset)).getTime()){
                    gold = 0;
                    tree = 0;
                }else{
                    var now = new Date(Date.now() - tzoffset);
                    now.setHours( now.getHours() + 6);
                    user.shield_end = now.toISOString();
                }
            }else{
                var now = new Date(Date.now() - tzoffset);
                now.setHours( now.getHours() + 6);
                user.shield_end = now.toISOString();
            }
            user.gold += gold;
            if(user.gold < 0){
                user.gold = 0;
            }
            user.tree += tree;
            if(user.tree < 0){
                user.tree = 0;
            }
            user.defence_record += req.body.enemyid + "/" + req.body.name + "/" + req.body.star + "/" + req.body.trophy + "/" + gold + "/" + tree + "_";
            
//            console.log("split start: " + user.defence_record);
            
            var array = user.defence_record.split("_");
            if(array.length > 5){
                var newStr = "";
                for(var i = 4; i >= 0; i--){
                    var str = array[array.length - i - 1];
                    if(str.length > 2){
                        newStr += str + "_";
                    }
                }
                user.defence_record = newStr;
            }
            user.save(function(err){
                if(err){
                    console.error("save error" + err);
                    res.json({error: "save failed"});
                    return;
                }else{
                    res.json({result: 1});
                }
            });
        });
    });
    // check playid
    app.post('/cc/checkplayid', function(req, res){
//        console.log("checkplayid data:" +  JSON.stringify(req.body));
        Ccuser.findOne({playid: req.body.playid}, function(err, userWithID){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(userWithID) { // user with the name exist
                res.json({result: userWithID.id});
            }else{
                res.json({result: "-1"});
            }
        });
    });
    // save user
    app.post('/cc/saveuser', function(req, res){
//        console.log("saveuser data:" +  JSON.stringify(req.body));
        var callback = function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(!user || req.body.id < 0) { // league data not found so create one
                Ccuser.countDocuments({} , function(err, result){
                    if(err){
                        console.log("err: " + err);
                        res.json({error: "count failed"});
                    }
                    // count is the result
                    var count = result;
                    
                    var user = new Ccuser();
                    user.id = "" + result;
//                    if(req.body.name) user.name = req.body.name;
                    
                    Ccuser.findOne({name: req.body.name}, function(err, userWithName){
                        if(err) return res.status(500).json({ error: 'database failure' });
                        if(userWithName) { // user with the name exist
//                            console.log("error same name exist: ");
                            res.json({error: "same name exist"});
                        }else{
                            user.name = req.body.name;
                            user.name = user.name.replace(",", ".");
                            user.name = user.name.replace("_", ".");
                            user.name = user.name.replace("/", ".");
                            var now = new Date(Date.now() - tzoffset).toISOString();
                            user.first_launch_date = now;
//                            console.log("new id: " + user.id);
                            user.save(function(err){
                                if(err){
                                    console.error(err);
                                    res.json({error: "save failed"});
                                }else{
                                    res.json({result: user.id, _id: user._id});
                                }
                            });
                        }
                    });
//                    res.json({result: user.id});
                });
//                return res.status(404).json({ error: 'user not found' });
            }else{
                if(req.body.id) user.id = req.body.id;
                if(req.body.name) user.name = req.body.name;
                if(req.body.password) user.password = req.body.password;
                if(req.body.playid) user.playid = req.body.playid;
                if(req.body.trophy) {
                    user.trophy = req.body.trophy;
                    var now = new Date(Date.now() - tzoffset).toISOString();
                    user.last_launch_date = now;
                }
                if(req.body.arena_clr_cnt) user.arena_clr_cnt = req.body.arena_clr_cnt;
                if(req.body.stage_clr_idx) user.stage_clr_idx = req.body.stage_clr_idx;
                if(req.body.spc_units) user.spc_units = req.body.spc_units;
                if(req.body.arena_reward) user.arena_reward = req.body.arena_reward;
                if(req.body.buildings) user.buildings = req.body.buildings;
                if(req.body.inventory) user.inventory = req.body.inventory;
                if(req.body.deck) user.deck = req.body.deck;
                if(req.body.hivt) {
                    user.hivt = req.body.hivt;
                    if(user.hivtbck.length < req.body.hivt.length){
                        user.hivtbck = req.body.hivt;
                    }
                }
                if(req.body.hivtbck) user.deck = req.body.hivtbck;
                if(req.body.hdck) user.hdck = req.body.hdck;
                if(req.body.keys) user.keys = req.body.keys;
                if(req.body.keygetstate) user.keygetstate = req.body.keygetstate;
                if(req.body.gem) user.gem = req.body.gem;
                if(req.body.gold) {
                    user.gold = req.body.gold;
                    if(user.gold < 0){
                        user.gold = 0;
                    }
                }
                if(req.body.tree) user.tree = req.body.tree;
                if(req.body.golden_ticket) user.golden_ticket = req.body.golden_ticket;
                if(req.body.defence_record) user.defence_record = req.body.defence_record;
                if(req.body.search_state) user.search_state = req.body.search_state;
                if(req.body.search_items) user.search_items = req.body.search_items;
                if(req.body.gold_chest) user.gold_chest = req.body.gold_chest;
                if(req.body.silver_chest) user.silver_chest = req.body.silver_chest;
                if(req.body.attend) user.attend = req.body.attend;
                if(req.body.gift_day) user.gift_day = req.body.gift_day;
                if(req.body.iap_list) user.iap_list = req.body.iap_list;
                if(req.body.iap_total) user.iap_total = req.body.iap_total;
                if(req.body.mission) user.mission = req.body.mission;
                if(req.body.post) user.post = req.body.post;
                if(req.body.ban) user.ban = req.body.ban;
                if(req.body.access_reward_receive) user.access_reward_receive = req.body.access_reward_receive;
                if(req.body.shield_end) user.shield_end = req.body.shield_end;
                if(req.body.last_launch_date) user.last_launch_date = req.body.last_launch_date;
                if(req.body.first_launch_date) user.first_launch_date = req.body.first_launch_date;
                if(req.body.pvp_rwd_6) user.pvp_rwd_6 = req.body.pvp_rwd_6;
                if(req.body.pvp_rwd_12) user.pvp_rwd_12 = req.body.pvp_rwd_12;
                if(req.body.pvp6Ticket) user.pvp6Ticket = req.body.pvp6Ticket;
                if(req.body.pvp12Ticket) user.pvp12Ticket = req.body.pvp12Ticket;
                if(req.body.pvp6_refill_start_time) user.pvp6_refill_start_time = req.body.pvp6_refill_start_time;
                if(req.body.pvp12_refill_start_time) user.pvp12_refill_start_time = req.body.pvp12_refill_start_time;
                if(req.body.monthlyEvent) user.monthlyEvent = req.body.monthlyEvent;
                
//                if(user.id == "5232" || user.id == "2"){
//                    var now = new Date(Date.now() - tzoffset).toISOString();
//                    console.log(now +" saveuser user.id + " + user.id+ "/" + req.body.id + "->" + user.buildings);
//                }
                user.save(function(err){
                    if(err){
                        console.error(err);
                        res.json({error: "save failed"});
                        return;
                    }
                });
                res.json({result: 1});
            };
        }
        var param;
        if(req.body._id){
            param = {_id: req.body._id}
        }else if(req.body.id){
            param = {id: req.body.id}
        }else{
            param = {name: req.body.name}
        }
        
        Ccuser.findOne(param, callback);
    });
    
    
    // get user data
    app.post('/cc/getuserdata', function(req, res){
//        console.log("get user data:" +  JSON.stringify(req.body));
        var param;
        if(req.body._id){
            param = {_id: req.body._id}
        }else if(req.body.id){
            param = {id: req.body.id}
        }else{
            param = {name: req.body.name}
        }
        Ccuser.findOne(param, function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(!user) {
                res.json({error: "user not found"});
            }else{
                var now = new Date(Date.now() - tzoffset).toISOString();
                var data = {time: now};//'{"time":' + now;
                
                if(req.body.id) {
                    data.id = user.id;
                }
                
                data._id = user._id;
                if(req.body.name) data.name = user.name;
                if(req.body.password) data.password = user.password;
                if(req.body.trophy) data.trophy = user.trophy;
                if(req.body.playid) data.playid = user.playid;
                if(req.body.arena_clr_cnt) data.arena_clr_cnt = user.arena_clr_cnt;
                if(req.body.stage_clr_idx) data.stage_clr_idx = user.stage_clr_idx;
                if(req.body.spc_units) data.spc_units = user.spc_units;
                if(req.body.arena_reward) data.arena_reward = user.arena_reward;
                if(req.body.buildings) data.buildings = user.buildings;
                if(req.body.inventory) data.inventory = user.inventory;
                if(req.body.deck) data.deck = user.deck;
                if(req.body.hivt) data.hivt = user.hivt;
                if(req.body.hdck) data.hdck = user.hdck;
                if(req.body.keys) data.keys = user.keys;
                if(req.body.keygetstate) data.keygetstate = user.keygetstate;
                if(req.body.gem) data.gem = user.gem;
                if(req.body.gold) data.gold = user.gold;
                if(req.body.tree) data.tree = user.tree;
                if(req.body.golden_ticket) data.golden_ticket = user.golden_ticket;
                if(req.body.search_state) data.search_state = user.search_state;
                if(req.body.search_items) data.search_items = user.search_items;
                if(req.body.gold_chest) data.gold_chest = user.gold_chest;
                if(req.body.silver_chest) data.silver_chest = user.silver_chest;
                if(req.body.defence_record) data.defence_record = user.defence_record;
                if(req.body.attend) data.attend = user.attend;
                if(req.body.gift_day) data.gift_day = user.gift_day;
                if(req.body.iap_list) data.iap_list = user.iap_list;
                if(req.body.iap_total) data.iap_total = user.iap_total;
                if(req.body.mission) data.mission = user.mission;
                if(req.body.post) data.post = user.post;
                if(req.body.ban) data.ban = user.ban;
                if(req.body.access_reward_receive) data.access_reward_receive = user.access_reward_receive;
                if(req.body.shield_end) data.shield_end = user.shield_end;
                if(req.body.last_launch_date) data.last_launch_date = user.last_launch_date;
                if(req.body.first_launch_date) data.first_launch_date = user.first_launch_date;
                if(req.body.pvp_rwd_6) data.pvp_rwd_6 = user.pvp_rwd_6;
                if(req.body.pvp_rwd_12) data.pvp_rwd_12 = user.pvp_rwd_12;
                if(req.body.pvp6Ticket) data.pvp6Ticket = user.pvp6Ticket;
                if(req.body.pvp12Ticket) data.pvp12Ticket = user.pvp12Ticket;
                if(req.body.pvp6_refill_start_time) data.pvp6_refill_start_time = user.pvp6_refill_start_time;
                if(req.body.pvp12_refill_start_time) data.pvp12_refill_start_time = user.pvp12_refill_start_time;
                if(req.body.monthlyEvent) data.monthlyEvent = user.monthlyEvent;
                
//                console.log("get user data result:" +  JSON.stringify(data));
//                user.last_launch_date = now; 
                user.save(function(err){
                    if(err){
                        console.error(err);
                    }
                });
                
                res.json(data);
            }
        });
    });
    // user login
    app.post('/cc/userlogin', function(req, res){
        Ccuser.findOne({name: req.body.name}, function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(!user) {
                res.json({error: "user not found"});
            }else{
                if(req.body.password === user.password){
                    res.json({result: user.id});
                }else{
                    res.json({error: "invalid password"});
                }
            }
        });
    });
    
    // user name change
    app.post('/cc/changename', function(req, res){
        Ccuser.findOne({name: req.body.name}, function(err, userWithName){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(userWithName) { // user with the name exist
                res.json({error: "same name exist"});
            }else{
                Ccuser.findOne({id: req.body.id}, function(err, user){
                    if(err) return res.status(500).json({ error: 'database failure' });
                    if(user){
                        user.name = req.body.name;
                        user.save(function(err){
                            if(err){
                                console.error(err);
                                res.json({error: "save failed"});
                            }else{
                                res.json({result: "sucess", name:req.body.name});
                            }
                        });
                    }else{
                        res.json({error: "cannot find user with id"});
                    }
                });
            }
        });
    });
    // get user
    app.post('/cc/getuser', function(req, res){
        var callback = function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });

            if(!user) { // user data not found
                res.json({error: "user not found"});
            }else{
                res.json(user);
            }
        }
        if(req.body.name){
//            console.log("name: " + req.body.name);
            Ccuser.findOne({name: req.body.name}, callback);
        }else{
//            console.log("id: " + req.body.id);
            Ccuser.findOne({id: req.body.id}, callback);
        }
    });
    
    // delete user
    app.post('/cc/deleteuser', function(req, res){
//        console.log("delete start");
        Ccuser.deleteOne({ id: req.body.id }, function (err) {
            if (err) {
                console.log("err: " + err);
            }
//            console.log("delete done");
            // deleted at most one tank document
        });
//        console.log("delete end");
    });
    
    // send post
    app.post('/cc/sendpost', function(req, res){
        var callback = function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });

            if(!user) { // user data not found
                res.json({error: "user not found"});
            }else{
                user.post += req.body.message + ",";
                user.save(function(err){
                    if(err){
                        console.error(err);
                        res.json({error: "save failed"});
                        return;
                    }
                    res.json("save success");
                });
            }
        }
        Ccuser.findOne({id: req.body.id}, callback);
    });
    
    // Ban
    app.post('/cc/ban', function(req, res){
        var callback = function(err, user){
            if(err) return res.status(500).json({ error: 'database failure' });
            if(!user) { // user data not found
                res.json({error: "user not found"});
            }else{
                user.ban = req.body.ban;
                user.save(function(err){
                    if(err){
                        console.error(err);
                        res.json({error: "save failed"});
                        return;
                    }
                    res.json("save success");
                });
            }
        }
        Ccuser.findOne({id: req.body.id}, callback);
    });
    
    // get arean rank
    app.post('/cc/getarenarank', function(req,res){
        return res.status(500).send({error: 'udpate game'});
//        console.log("rank req id: " + req.body.id);
//        Ccuser.find({arena_score:{$gt:0}}).sort({arena_score : 1}) // sort -1 for ascending
////                                            .limit(Number(req.body.count))
//                                            .exec(function(err, users){
//            if(err) return res.status(500).send({error: 'database failure'});
////            console.log("found users for match: " + users.length);
//            var names = "";
//            var you;
//            var str;
//            for(var i = 0; i < users.length; i++){
//                str = "";
//                if(i < 100){
//                     str = users[i].name + "_" + users[i].arena_score + "_" + users[i].id + ",";
//                }
////                console.log(i + ". " + str);
//                if(str.length > 0){
//                    names += str;
//                }
//                if(users[i].id == req.body.id){
//                    you = i;
//                }
//            }
//            
//            res.json({names:names, you:you, total:users.length});
//        });
    });
}
