[CCode (cheader_filename="cJSON.h", cname="cJSON", free_function="cJSON_Delete")]
[Compact]
class cJSON {
	[CCode (cname="int")]
	public enum Type {
		[CCode (cname="cJSON_False")]
		False,
		[CCode (cname="cJSON_True")]
		True,
		[CCode (cname="cJSON_NULL")]
		NULL,
		[CCode (cname="cJSON_Number")]
		Number,
		[CCode (cname="cJSON_String")]
		String,
		[CCode (cname="cJSON_Array")]
		Array,
		[CCode (cname="cJSON_Object")]
		Object
	}

	public cJSON next;
	public cJSON prev;
	public cJSON child;
	public Type type;

	[CCode (cname="valuestring")]
	public string @string;
	[CCode (cname="valueint")]
	public int @int;
	[CCode (cname="valuedouble")]
	public double @double;

	[CCode (cname="string")]
	public string name;

	[CCode (cname="cJSON_Parse")]
	public static cJSON parse(string value);

	[CCode (cname="cJSON_GetArraySize")]
	public int arraySize();
	[CCode (cname="cJSON_GetArrayItem")]
	public unowned cJSON arrayItem(int i);
	[CCode (cname="cJSON_GetObjectItem")]
	public unowned cJSON objectItem(string key);
}
