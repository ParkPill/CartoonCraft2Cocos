/* Copyright (C) 2014 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cocos2dx.cpp;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.SharedPreferences;
import android.os.Parcel;

//import com.google.android.gms.drive.Contents;
//import com.google.android.gms.games.snapshot.Snapshot;
//import com.google.android.gms.games.snapshot.SnapshotMetadata;

/**
 * Represents the player's progress in the game. The player's progress is how many value
 * they got on each level.
 *
 * @author Bruno Oliveira
 */
public class SaveGame{

    private static final String TAG = "Saved Game";

    // serialization format version
    private static final String SERIAL_VERSION = "1.1";

    // Maps a level name (like "2-8") to the number of value the user has in that level.
    // Any key that doesn't exist in this map is considered to be associated to the value 0.
    Map<String,String> mKeyValueDic = new HashMap<String,String>();
    /** Constructs an empty SaveGame object. No value on no levels. */
    public SaveGame() {
    }

    /** Constructs a SaveGame object from serialized data. */
    public SaveGame(byte[] data) {
        if (data == null) return; // default progress
        loadFromJson(new String(data));
    }

    /** Constructs a SaveGame object from a JSON string. */
    public SaveGame(String json) {
        if (json == null) return; // default progress
        loadFromJson(json);
    }

    /** Constructs a SaveGame object by reading from a SharedPreferences. */
    public SaveGame(SharedPreferences sp, String key) {
        loadFromJson(sp.getString(key, ""));
    }

    /** Replaces this SaveGame's content with the content loaded from the given JSON string. */
    public void loadFromJson(String json) {
        zero();
        if (json == null || json.trim().equals("")) return;

        try {
            JSONObject obj = new JSONObject(json);
            String format = obj.getString("version");
            if (!format.equals(SERIAL_VERSION)) {
                throw new RuntimeException("Unexpected loot format " + format);
            }
            JSONObject levels = obj.getJSONObject("levels");
            Iterator<?> iter = levels.keys();

            while (iter.hasNext()) {
                String key = (String)iter.next();
                mKeyValueDic.put(key, levels.getString(key));
            }
        }
        catch (JSONException ex) {
            ex.printStackTrace();
            Log.e(TAG, "Save data has a syntax error: " + json, ex);

            // Initializing with empty value if the game file is corrupt.
            // NOTE: In your game, you want to try recovering from the snapshot payload.
            mKeyValueDic.clear();
        }
        catch (NumberFormatException ex) {
            ex.printStackTrace();
            throw new RuntimeException("Save data has an invalid number in it: " + json, ex);
        }
    }

    /** Serializes this SaveGame to an array of bytes. */
    public byte[] toBytes() {
        return toString().getBytes();
    }

    /** Serializes this SaveGame to a JSON string. */
    @Override
    public String toString() {
        try {
            JSONObject levels = new JSONObject();
            for (String key : mKeyValueDic.keySet()) {
                levels.put(key, mKeyValueDic.get(key));
            }

            JSONObject obj = new JSONObject();
            obj.put("version", SERIAL_VERSION);
            obj.put("levels", levels);
            return obj.toString();
        }
        catch (JSONException ex) {
            ex.printStackTrace();
            throw new RuntimeException("Error converting save data to JSON.", ex);
        }
    }

    /**
     * Computes the union of this SaveGame with the given SaveGame. The union will have any
     * levels present in either operand. If the same level is present in both operands,
     * then the number of value will be the greatest of the two.
     *
     * @param other The other operand with which to compute the union.
     * @return The result of the union.
     */
    public SaveGame unionWith(SaveGame other) {
        SaveGame result = clone();
        String versionKey = "snapShotVersion";
        for (String key : other.mKeyValueDic.keySet()) {
            // only overwrite if number of value is greater
            if (Integer.parseInt(result.getValue(versionKey)) < Integer.parseInt(other.getValue(versionKey))) {
                result.setStringValue(key, other.getValue(key));
            }
        }
        return result;
    }

    /** Returns a clone of this SaveGame object. */
    public SaveGame clone() {
        SaveGame result = new SaveGame();
        for (String key : mKeyValueDic.keySet()) {
            result.setStringValue(key, getValue(key));
        }
        return result;
    }

    /** Resets this SaveGame object to be empty. Empty means no value on no levels. */
    public void zero() {
        mKeyValueDic.clear();
    }

    /** Returns whether or not this SaveGame is empty. Empty means no value on no levels. */
    public boolean isZero() {
        return mKeyValueDic.keySet().size() == 0;
    }

    /** Save this SaveGame object to a SharedPreferences. */
    public void save(SharedPreferences sp, String key) {
        SharedPreferences.Editor spe = sp.edit();
        spe.putString(key, toString());
        spe.commit();
    }

    public void setStringValue(String key, String value) {
        if (value == null || value.isEmpty()) {
            // zero value means remove it from the map
            if (mKeyValueDic.containsKey(key)) {
                mKeyValueDic.remove(key);
            }
        } else {
            mKeyValueDic.put(key, value);
        }
    }
    public void setIntValue(String key, int value) {
        mKeyValueDic.put(key, Integer.toString(value));
        /*if (value == 0) {
            // zero value means remove it from the map
            if (mKeyValueDic.containsKey(key)) {
                mKeyValueDic.remove(key);
            }
        } else {
            mKeyValueDic.put(key, Integer.toString(value));
        }*/
    }
    public String getValue(String key) {
        return mKeyValueDic.get(key);
    }

    /**  Implementation of Snapshot interface.  */
    /*
    @Override
    public byte[] readFully() {
        return new byte[0];
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {

    }

    @Override
    public boolean writeBytes(byte[] bytes) {
        return false;
    }

    @Override
    public void jy() {

    }

    @Override
    public boolean modifyBytes(int i, byte[] bytes, int i2, int i3) {
        return false;
    }

    @Override
    public boolean isDataValid() {
        return false;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public Snapshot freeze() {
        return null;
    }

    @Override
    public SnapshotMetadata getMetadata() {
        return null;
    }

    @Override
    public Contents getContents() {
        return null;
    }*/
}
