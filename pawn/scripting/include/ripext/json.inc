// Encoding flags
#define JSON_MAX_INDENT         0x1F		/**< Maximum indentation */
#define JSON_INDENT(%1)         ((%1) & JSON_MAX_INDENT)		/**< Pretty-print the result and indent with n spaces */
#define JSON_COMPACT            0x20		/**< Compact representation */
#define JSON_ENSURE_ASCII       0x40		/**< Escape all Unicode characters outside the ASCII range */
#define JSON_SORT_KEYS          0x80		/**< Sort object keys */
#define JSON_ENCODE_ANY         0x200		/**< Encode any value */
#define JSON_ESCAPE_SLASH       0x400		/**< Escape / with \/ */
#define JSON_REAL_PRECISION(%1) (((%1) & 0x1F) << 11)		/**< Output floats with at most n digits of precision */
#define JSON_EMBED              0x10000		/**< Omit opening and closing braces of the top-level object */

// Generic type for encoding JSON.
methodmap JSON < Handle
{
	// Writes the JSON string representation of the object to a file.
	//
	// @param file       File to write to.
	// @param flags      Encoding flags.
	// @return           True on success, false on failure.
	public native bool ToFile(const char[] file, int flags = JSON_INDENT(4));

	// Retrieves the JSON string representation of the object.
	//
	// @param buffer     String buffer to write to.
	// @param maxlength  Maximum length of the string buffer.
	// @param flags      Encoding flags.
	// @return           True on success, false on failure.
	public native bool ToString(char[] buffer, int maxlength, int flags = 0);
};

methodmap JSONObject < JSON
{
	// Creates a JSON object. A JSON object maps strings (called "keys") to values. Keys in a
	// JSON object are unique. That is, there is at most one entry in the map for a given key.
	//
	// The JSONObject must be freed via delete or CloseHandle().
	public native JSONObject();

	// Loads a JSON object from a file.
	//
	// @param file       File to read from.
	// @return           Object handle, or null on failure.
	// @error            Invalid JSON.
	public static native JSONObject FromFile(const char[] file);

	// Loads a JSON object from a string.
	//
	// @param buffer     String buffer to load into the JSON object.
	// @return           Object handle, or null on failure.
	// @error            Invalid JSON.
	public static native JSONObject FromString(const char[] buffer);

	// Retrieves an array or object value from the object.
	//
	// @param key        Key string.
	// @return           Value read.
	// @error            Invalid key.
	public native JSON Get(const char[] key);

	// Retrieves a boolean value from the object.
	//
	// @param key        Key string.
	// @return           Value read.
	// @error            Invalid key.
	public native bool GetBool(const char[] key);

	// Retrieves a float value from the object.
	//
	// @param key        Key string.
	// @return           Value read.
	// @error            Invalid key.
	public native float GetFloat(const char[] key);

	// Retrieves an integer value from the object.
	//
	// @param key        Key string.
	// @return           Value read.
	// @error            Invalid key.
	public native int GetInt(const char[] key);

	// Retrieves a string value from the object.
	//
	// @param key        Key string.
	// @param buffer     String buffer to store value.
	// @param maxlength  Maximum length of the string buffer.
	// @return           True on success. False if the key was not found, or the value is not a string.
	public native bool GetString(const char[] key, char[] buffer, int maxlength);

	// Returns whether or not a value in the object is null.
	//
	// @param key        Key string.
	// @return           True if the value is null, false otherwise.
	// @error            Invalid key.
	public native bool IsNull(const char[] key);

	// Returns whether or not a key in the object exists
	//
	// @param key        Key string.
	// @return           True if the key exists, false otherwise.
	public native bool HasKey(const char[] key);

	// Sets an array or object value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	public native bool Set(const char[] key, JSON value);

	// Sets a boolean value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	public native bool SetBool(const char[] key, bool value);

	// Sets a float value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	public native bool SetFloat(const char[] key, float value);

	// Sets an integer value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	public native bool SetInt(const char[] key, int value);

	// Sets a null value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @return           True on success, false on failure.
	public native bool SetNull(const char[] key);

	// Sets a string value in the object, either inserting a new entry or replacing an old one.
	//
	// @param key        Key string.
	// @param value      Value to store at this key.
	// @return           True on success, false on failure.
	public native bool SetString(const char[] key, const char[] value);

	// Removes an entry from the object.
	//
	// @param key        Key string.
	// @return           True on success, false if the key was not found.
	public native bool Remove(const char[] key);

	// Clears the object of all entries.
	// @return           True on success, false on failure.
	public native bool Clear();

	// Retrieves the size of the object.
	property int Size {
		public native get();
	}
};

methodmap JSONArray < JSON
{
	// Creates a JSON array.
	//
	// The JSONArray must be freed via delete or CloseHandle().
	public native JSONArray();

	// Loads a JSON array from a file.
	//
	// @param file       File to read from.
	// @return           Array handle, or null on failure.
	// @error            Invalid JSON.
	public static native JSONArray FromFile(const char[] file);

	// Loads a JSON array from a string.
	//
	// @param buffer     String buffer to load into the JSON array.
	// @return           Array handle, or null on failure.
	// @error            Invalid JSON.
	public static native JSONArray FromString(const char[] buffer);

	// Retrieves an array or object value from the array.
	//
	// @param index      Index in the array.
	// @return           Value read.
	// @error            Invalid index.
	public native JSON Get(int index);

	// Retrieves a boolean value from the array.
	//
	// @param index      Index in the array.
	// @return           Value read.
	// @error            Invalid index.
	public native bool GetBool(int index);

	// Retrieves a float value from the array.
	//
	// @param index      Index in the array.
	// @return           Value read.
	// @error            Invalid index.
	public native float GetFloat(int index);

	// Retrieves an integer value from the array.
	//
	// @param index      Index in the array.
	// @return           Value read.
	// @error            Invalid index.
	public native int GetInt(int index);

	// Retrieves a string value from the array.
	//
	// @param index      Index in the array.
	// @param buffer     Buffer to copy to.
	// @param maxlength  Maximum size of the buffer.
	// @return           True on success, false if the value is not a string.
	// @error            Invalid index.
	public native bool GetString(int index, char[] buffer, int maxlength);

	// Returns whether or not a value in the array is null.
	//
	// @param index      Index in the array.
	// @return           True if the value is null, false otherwise.
	// @error            Invalid index.
	public native bool IsNull(int index);

	// Sets an array or object value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @return           True on success, false on failure.
	public native bool Set(int index, JSON value);

	// Sets a boolean value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @return           True on success, false on failure.
	public native bool SetBool(int index, bool value);

	// Sets a float value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @return           True on success, false on failure.
	public native bool SetFloat(int index, float value);

	// Sets an integer value in the array.
	//
	// @param index      Index in the array.
	// @param value      Value to set.
	// @return           True on success, false on failure.
	public native bool SetInt(int index, int value);

	// Sets a null value in the array.
	//
	// @param index      Index in the array.
	// @return           True on success, false on failure.
	public native bool SetNull(int index);

	// Sets a string value in the array.
	//
	// @param index      Index in the array.
	// @param value      String value to set.
	// @return           True on success, false on failure.
	public native bool SetString(int index, const char[] value);

	// Pushes an array or object value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @return           True on success, false on failure.
	public native bool Push(JSON value);

	// Pushes a boolean value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @return           True on success, false on failure.
	public native bool PushBool(bool value);

	// Pushes a float value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @return           True on success, false on failure.
	public native bool PushFloat(float value);

	// Pushes an integer value onto the end of the array, adding a new index.
	//
	// @param value      Value to push.
	// @return           True on success, false on failure.
	public native bool PushInt(int value);

	// Pushes a null value onto the end of the array, adding a new index.
	// @return           True on success, false on failure.
	public native bool PushNull();

	// Pushes a string value onto the end of the array, adding a new index.
	//
	// @param value      String value to push.
	// @return           True on success, false on failure.
	public native bool PushString(const char[] value);

	// Removes an entry from the array.
	//
	// @param index      Index in the array to remove.
	// @return           True on success, false on invalid index.
	public native bool Remove(int index);

	// Clears the array of all entries.
	// @return           True on success, false on failure.
	public native bool Clear();

	// Retrieves the size of the array.
	property int Length {
		public native get();
	}
};
